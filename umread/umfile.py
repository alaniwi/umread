import os
import numpy


class UMFile(object):

    """
    a class for a UM data file that gives a view of the file including sets of
    PP records combined into variables
    """
    
    def __init__(self, path,
                 byte_ordering = None,
                 wordsize = None,
                 format = None):
        
        """
        Open and parse a UM file.  The following optional arguments specify
        the file type.  If all three are set, then this forces the file type;
        otherwise, the file type is autodetected and any of them that are set 
        are ignored.

           byte_ordering: 'little_endian' or 'big_endian'
           wordsize: 4 or 8
           format:  'FF' or 'PP'
        """

        if byte_ordering and wordsize and format:
            self.byte_ordering = byte_ordering
            self.wordsize = wordsize
            self.format = format
        else:
            self._detect_file_type()
        self.path = path
        self.open_fh()
        self._parse_file()
    
    def open_fh(self):
        """
        (Re)open the low-level file handle.
        """
        self.fh = os.open(path, os.O_RDONLY)
        return self.fh

    def close_fh(self):
        """
        Close the low-level file handle.
        """
        if self.fh:
            os.close(self.fh)
        self.fh = None

    def _detect_file_type(self):
        pass

    def _parse_file(self):
        pass


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
    def __init__(self, int_hdr, real_hdr, hdr_offset, data_offset):
        self.int_hdr = int_hdr
        self.real_hdr = real_hdr
        self.hdr_offset = hdr_offset
        self.data_offset = data_offset
