import os
import numpy
import numpy.ctypeslib
import ctypes as CT

import umfile
 
class File_type(CT.Structure):
    _fields_ = [("format", CT.c_int),
                ("byte_ordering", CT.c_int),
                ("word_size", CT.c_int)]

class Rec(CT.Structure):
    """
    ctypes object corresponding to the Rec object in the C code
    """
    # defer setting _fields_ as this depends on 
    # file data type for correct interpretation of the 
    # void* used for each of int and real PP header data
    pass

class Var(CT.Structure):
    """
    ctypes object corresponding to the Var object in the C code
    """
    _fields_ = [("recs", CT.POINTER(CT.POINTER(Rec))),
                ("nz", CT.c_int),
                ("nt", CT.c_int),
                ("supervar_index", CT.c_int),
                ("_internp", CT.c_void_p)]

class File(CT.Structure):
    """
    ctypes object corresponding to the File object in the C code
    """
    _fields_ = [("fd", CT.c_int),
                ("file_type", File_type),
                ("nvars", CT.c_int),
                ("vars", CT.POINTER(CT.POINTER(Var))),
                ("_internp", CT.c_void_p)]

class Enum(object):
    def __init__(self, names):
        self.names = names
    def as_name(self, val):
        if isinstance(val, str):
            return val
        else:
            return self.names[val]
    def as_index(self, val):
        if isinstance(val, int):
            return val
        return self.index(val)


enum_file_format = Enum(("PP", "FF"))
enum_byte_ordering = Enum(("little_endian", "big_endian"))

class CInterface(object):
    """
    Interface to the C shared library functions
    """
    def __init__(self, lib_name = "umfile_test.so"):
        lib_dir = os.path.dirname(__file__) or "."
        lib_path = os.path.join(lib_dir, lib_name)
        print lib_path
        self.lib = CT.CDLL(lib_path)
    
    def detect_file_type(self, fd):
        """
        auto-detect file type; returns a File_type ctypes object 
        that can be passed to file_parse(), or 
        raises an exception if file type cannot be detected
        """
        file_type = File_type()
        rv = self.lib.detect_file_type(fd, CT.pointer(file_type))
        if rv != 0:
            raise RuntimeError("File type could not be detected")
        return file_type

    def file_type_obj_to_dict(self, file_type):
        """
        converts a FileType object returned by detect_file_type()
        into a dictionary that include meaningful string 
        values in place of the integers that derive from the C enum
        statments, specifically:
           'format': 'PP' or 'FF'
           'byte_ordering': 'little_endian' or 'big_endian'
        and also
           'word_size': 4 or 8
        """
        format = enum_file_format.as_name(file_type.format)
        byte_ordering = enum_byte_ordering.as_name(file_type.byte_ordering)
        word_size = file_type.word_size
        return {
            'format': format,
            'byte_ordering': byte_ordering,
            'word_size': word_size
            }

    def create_file_type(self, format, byte_ordering, word_size):
        """
        takes string input values:
           'format': 'PP' or 'FF'
           'byte_ordering': 'little_endian' or 'big_endian'
           'word_size': 4 or 8    
        and returns a FileType object (ctypes structure containing integer
        values) that can be passed to file_parse()
        """
        file_type = File_type(format = enum_file_format.as_index(format), 
                              byte_ordering = enum_byte_ordering.as_index(byte_ordering), 
                              word_size = word_size)

    def set_word_size(self, val):
        """
        Sets the word size used to interpret returned pointers from subsequent
        calls, in particular the pointers to PP headers embedded in the tree of
        objects returned by file_parse() and the data array that is populated
        by read_record_data().

        the 'val' argument contains either just the word_size value to use or a 
        file_type object from which it is to be extracted
        """
        if isinstance(val, File_type):
            word_size = val.word_size
        else:
            word_size = val

        if word_size == 4:
            self.file_data_int_type = numpy.int32
            self.file_data_real_type = numpy.float32
        elif word_size == 8:
            self.file_data_int_type = numpy.int64
            self.file_data_real_type = numpy.float64
        else:
            raise ValueError("word size must be 4 or 8")            

        ctypes_int_hdr = numpy.ctypeslib.ndpointer(dtype = self.file_data_int_type,
                                                   shape = (45),
                                                   flags=('C_CONTIGUOUS', 'WRITEABLE'))
        
        ctypes_real_hdr = numpy.ctypeslib.ndpointer(dtype = self.file_data_real_type,
                                                    shape = (19),
                                                    flags=('C_CONTIGUOUS', 'WRITEABLE'))
        
        Rec._fields_ = [("int_hdr", ctypes_int_hdr),
                        ("real_hdr", ctypes_real_hdr),
                        ("header_offset", CT.c_size_t),
                        ("data_offset", CT.c_size_t),
                        ("_internp", CT.c_void_p)]

        self.ctypes_int_data = numpy.ctypeslib.ndpointer(dtype = self.file_data_int_type,
                                                         ndim = 1,
                                                         flags=('C_CONTIGUOUS', 'WRITEABLE'))

        self.ctypes_real_data = numpy.ctypeslib.ndpointer(dtype = self.file_data_real_type,
                                                          ndim = 1,
                                                          flags=('C_CONTIGUOUS', 'WRITEABLE'))

    def parse_file(self, fh, file_type):
        """
        Given an open file handle, work out information from the file, and
        return this in a dictionary, of which currently the only key actually
        implemented is 'vars', containing a list of variables, as that is all 
        that the caller requires.

        arguments: fh - low-level file handle (integer)
                   file_type - File_type object as returned by detect_file_type() or 
                               create_file_type()                               
        """
        func = self.lib.file_parse
        func.restype = CT.POINTER(File)
        file_p = func(fh, file_type)
        file = file_p.contents
        c_vars = file.vars[:file.nvars]
        rv = {'vars': map(self.c_var_to_py_var, c_vars)}
        return rv

    def c_var_to_py_var(self, c_var_p):
        """
        create a umfile.Var object from a ctypes object corresponding to 'Var*' 
        in the C code
        """
        c_var = c_var_p.contents
        nz = c_var.nz
        nt = c_var.nt
        svi = c_var.supervar_index
        if svi < 0:
            svi = None
        c_recs = c_var.recs
        recs = [self.c_rec_to_py_rec(c_recs[recid]) for recid in range(nz * nt)]
        return umfile.Var(recs, nz, nt, svi)

    def c_rec_to_py_rec(self, c_rec_p):
        """
        create a umfile.Rec object from a ctypes object corresponding to 'Rec*' 
        in the C code
        """
        c_rec = c_rec_p.contents
        int_hdr = numpy.ctypeslib.as_array(c_rec.int_hdr)
        real_hdr = numpy.ctypeslib.as_array(c_rec.real_hdr)
        header_offset = c_rec.header_offset
        data_offset = c_rec.data_offset
        return umfile.Rec(int_hdr, real_hdr, header_offset, data_offset)

    def get_nwords(self, int_hdr):
        """
        from integer header, work out number of words to read
        (read_record_data requires this)
        """
        word_size = int_hdr.itemsize
        return self.lib.get_nwords(word_size, numpy.ctypeslib.as_ctypes(int_hdr))

    def read_record_data(self,
                         fd,
                         data_offset,
                         byte_ordering, 
                         word_size,
                         int_hdr,
                         real_hdr,
                         nwords,
                         data_is_int=False):
        """
        reads record data from open file

        inputs:
           fd - integer low-level file descriptor
           data_offset - offset in words
           byte_ordering - 'little_endian' or 'big_endian'
           word_size - 4 or 8
           int_hdr - integer PP headers (numpy array)
           real_hdr - real PP headers (numpy array)
           nwords - number of words to read, should have been returned by get_nwords()

        optional input:
          data_is_int: set True if data is integer, 
                       otherwise data assumed to be real
        """
        
        if data_is_int:
            ctypes_data = self.ctypes_int_data
            data = numpy.empty(nwords, dtype = self.file_data_int_type)
        else:
            ctypes_data = self.ctypes_real_data
            data = numpy.empty(nwords, dtype = self.file_data_real_type)

        self.lib.read_record_data.argtypes = [ CT.c_int,
                                               CT.c_size_t,
                                               CT.c_int,
                                               CT.c_int,
                                               CT.c_void_p,
                                               CT.c_void_p,
                                               CT.c_size_t,
                                               ctypes_data ]
        self.lib.read_record_data(fd,
                                  data_offset,
                                  enum_byte_ordering.as_index(byte_ordering),
                                  word_size,
                                  numpy.ctypeslib.as_ctypes(int_hdr),
                                  numpy.ctypeslib.as_ctypes(real_hdr),
                                  nwords,
                                  data)

        return data


#FIXME:
#add read_header method, and in the dummy C code (is in the header file)
#add C method to parse int hdr and get data type (i.e. whether data is integer or real)


if __name__ == "__main__":
    c = CInterface()
    fd = 3
    file_type = c.detect_file_type(fd)
    print c.file_type_obj_to_dict(file_type)
    c.set_word_size(file_type)
    info = c.parse_file(fd, file_type)

    for var in info['vars']:
        print "nz = %s, nt = %s" % (var.nz, var.nt)
        for rec in var.recs:
            print rec.hdr_offset
            print rec.data_offset
            print rec.int_hdr
            print rec.real_hdr
            nwords = c.get_nwords(rec.int_hdr)
            data = c.read_record_data(fd,
                                      rec.data_offset,
                                      file_type.byte_ordering,
                                      file_type.word_size,
                                      rec.int_hdr,
                                      rec.real_hdr,
                                      nwords)
            print data
