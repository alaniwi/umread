import os

import cInterface

class UMFileException(Exception):
    pass

class File(object):

    """
    a class for a UM data file that gives a view of the file including sets of
    PP records combined into variables
    """
    
    def __init__(self, path,
                 byte_ordering = None,
                 word_size = None,
                 format = None,
                 parse = True):
        
        """
        Open and parse a UM file.  The following optional arguments specify
        the file type.  If all three are set, then this forces the file type;
        otherwise, the file type is autodetected and any of them that are set 
        are ignored.

           byte_ordering: 'little_endian' or 'big_endian'
           word_size: 4 or 8
           format:  'FF' or 'PP'

        The default action is to open the file, store the file type from the
        arguments or autodetection as described above, and then parse the
        contents, giving a tree of variables and records under the File
        object.  However, if "parse = False" is set, then an object is
        returned in which the last step is omitted, so only the file type is
        stored, and there are no variables under it.  Such an object can be
        passed when instantiating Rec objects, and contains sufficient info
        about the file type to ensure that the get_data method of those Rec
        objects will work.
        """
        c = cInterface.CInterface()
        self.c_interface = c
        self.path = path
        self.open_fd()
        if byte_ordering and word_size and format:
            self.format = format
            self.byte_ordering = byte_ordering
            self.word_size = word_size
        else:
            self._detect_file_type()
        self.path = path
        file_type_obj = c.create_file_type(self.format, self.byte_ordering, self.word_size)
        c.set_word_size(file_type_obj)
        if parse:
            info = c.parse_file(self.fd, file_type_obj)
            self.vars = info["vars"]
            self._add_back_refs()
    
    def open_fd(self):
        """
        (Re)open the low-level file descriptor.
        """
        self.fd = os.open(self.path, os.O_RDONLY)
        return self.fd

    def close_fd(self):
        """
        Close the low-level file descriptor.
        """
        if self.fd:
            os.close(self.fd)
        self.fd = None

    def _detect_file_type(self):
        c = self.c_interface
        file_type_obj = c.detect_file_type(self.fd)
        d = c.file_type_obj_to_dict(file_type_obj)
        self.format = d["format"]
        self.byte_ordering = d["byte_ordering"]
        self.word_size = d["word_size"]

    def _add_back_refs(self):
        """
        Add file attribute to Var objects, and both file and var attributes
        to Rec objects.  The important one is the file attribute in the Rec
        object, as this is used when reading data.  The others are provided
        for extra convenience.
        """
        for var in self.vars:
            var.file = self
            for rec in var.recs:
                rec.var = var
                rec.file = self

class Var(object):
    """
    container for some information about variables
    """
    def __init__(self, recs, nz, nt, supervar_index=None):
        self.recs = recs
        self.nz = nz
        self.nt = nt
        self.supervar_index = supervar_index


class Rec(object):
    """
    container for some information about records
    """
    def __init__(self, int_hdr, real_hdr, hdr_offset, data_offset, disk_length, 
                 file = None):
        """
        Default instantiation, which stores the supplied headers and offsets.
        The 'file' argument, used to set the 'file' attribute, does not need
        to be supplied, but if it is not then it will have to be set on the
        returned object (to the containing File object) before calling
        get_data() will work.  Normally this would be done by the calling code
        instantiating via File rather than directly.
        """
        self.int_hdr = int_hdr
        self.real_hdr = real_hdr
        self.hdr_offset = hdr_offset
        self.data_offset = data_offset
        self.disk_length = disk_length
        if file:
            self.file = file

    @classmethod
    def from_file_and_offsets(cls, file, hdr_offset, data_offset, disk_length):
        """
        Instantiate a Rec object from the file object and the 
        header and data offsets.  The headers are read in, and 
        also the record object is ready for calling get_data().
        """
        c = file.c_interface
        int_hdr, real_hdr = c.read_header(file.fd,
                                          hdr_offset,
                                          file.byte_ordering,
                                          file.word_size)
        return cls(int_hdr, real_hdr, hdr_offset, data_offset, disk_length, file=file)

    def get_data(self):
        """
        get the data array associated with the record
        """
        c = self.file.c_interface
        file = self.file
        data_type, nwords = c.get_type_and_length(self.int_hdr)
        print "data_type = %s nwords = %s" % (data_type, nwords)
        return c.read_record_data(file.fd,
                                  self.data_offset,
                                  self.disk_length,
                                  file.byte_ordering,
                                  file.word_size,
                                  self.int_hdr,
                                  self.real_hdr,
                                  data_type, 
                                  nwords)

if __name__ == '__main__':

    path = "test.pp"
    #path = "/tmp/xjaroa.pj1991mar"
    #path = "/tmp/xjaroa.pj1991mar.le"
    f = File(path)
    print f.format, f.byte_ordering, f.word_size
    print len(f.vars)
    for var in f.vars:
        print "nz = %s, nt = %s" % (var.nz, var.nt)
        for rec in var.recs:
            print "hdr offset: %s" % rec.hdr_offset
            print "data offset: %s" % rec.data_offset
            print "disk length: %s" % rec.disk_length
            print "int hdr: %s" % rec.int_hdr
            print "real hdr: %s" % rec.real_hdr
            print "data: %s" % rec.get_data()
    f.close_fd()
    
    # also read a record using saved metadata
    format = f.format
    byte_ordering = f.byte_ordering
    word_size = f.word_size
    myrec = f.vars[0].recs[0]
    hdr_offset = myrec.hdr_offset
    data_offset = myrec.data_offset
    disk_length = myrec.disk_length
    
    del(f)

    fnew = File(path,
                format = format,
                byte_ordering = byte_ordering,
                word_size = word_size,
                parse = False)

    rnew = Rec.from_file_and_offsets(fnew, hdr_offset, data_offset, disk_length)
    print "record read using saved file type and offsets:"
    print "int hdr: %s" % rnew.int_hdr
    print "real hdr: %s" % rnew.real_hdr
    print "data: %s" % rnew.get_data()
