import numpy
import numpy.ctypeslib
import ctypes as CT
mylib = CT.CDLL("./umfile_test.so") 

fd = 3

class File_type(CT.Structure):
    _fields_ = [("format", CT.c_int),
                ("byte_ordering", CT.c_int),
                ("word_size", CT.c_int)]
    
file_type = File_type()

assert(mylib.detect_file_type(fd, CT.pointer(file_type)) == 0)

assert(file_type.word_size == 4)
print "word size = %d" % file_type.word_size

my_int_type = CT.c_int
my_float_type = CT.c_float

my_int_type_numpy = numpy.int32
my_float_type_numpy = numpy.float32

ctypes_int_hdr = numpy.ctypeslib.ndpointer(dtype = my_int_type_numpy,
                                           shape = (45),
                                           flags=('C_CONTIGUOUS', 'WRITEABLE'))

ctypes_real_hdr = numpy.ctypeslib.ndpointer(dtype = my_float_type_numpy,
                                            shape = (19),
                                            flags=('C_CONTIGUOUS', 'WRITEABLE'))

ctypes_data = numpy.ctypeslib.ndpointer(dtype = my_float_type_numpy,
                                        ndim = 1,
                                        flags=('C_CONTIGUOUS', 'WRITEABLE'))

class Rec(CT.Structure):
    _fields_ = [("int_hdr", ctypes_int_hdr),
                ("real_hdr", ctypes_real_hdr),
                ("header_offset", CT.c_size_t),
                ("data_offset", CT.c_size_t),
                ("disk_length", CT.c_size_t),
                ("_internp", CT.c_void_p)]

class Var(CT.Structure):
    _fields_ = [("recs", CT.POINTER(CT.POINTER(Rec))),
                ("nz", CT.c_int),
                ("nt", CT.c_int),
                ("supervar_index", CT.c_int),
                ("_internp", CT.c_void_p)]

class File(CT.Structure):
    _fields_ = [("fd", CT.c_int),
                ("file_type", File_type),
                ("nvars", CT.c_int),
                ("vars", CT.POINTER(CT.POINTER(Var))),
                ("_internp", CT.c_void_p)]

func = mylib.file_parse
func.restype = CT.POINTER(File)
file = func(fd, file_type)

mylib.read_record_data.argtypes = [ CT.c_int,
                                    CT.c_size_t,
                                    CT.c_int,
                                    CT.c_int,
                                    CT.c_void_p,
                                    CT.c_void_p,
                                    CT.c_size_t,
                                    ctypes_data ]


for varid in range(file.contents.nvars):
    var = file.contents.vars[varid]
    print "nz = %s, nt = %s" % (var.contents.nz, var.contents.nt)
    nrec = var.contents.nz * var.contents.nt
    for recid in range(nrec):
        rec = var.contents.recs[recid]
        print "var %s rec %s" % (varid, recid)
        #ihdr = CT.cast(rec.contents.int_hdr, CT.POINTER(my_int_type))
        #rhdr = CT.cast(rec.contents.real_hdr, CT.POINTER(my_float_type))
        #print "int header: ", [ihdr[k] for k in range(45)]
        #print "real header: ", [rhdr[k] for k in range(19)]
        int_hdr = numpy.ctypeslib.as_array(rec.contents.int_hdr)
        print "int header: ", int_hdr, int_hdr.dtype

        real_hdr = numpy.ctypeslib.as_array(rec.contents.real_hdr)
        print "real header: ", real_hdr, real_hdr.dtype

        word_size = file_type.word_size
        nwords = mylib.get_nwords(word_size, numpy.ctypeslib.as_ctypes(int_hdr))
        print "data (%s items)" % nwords
        data = numpy.empty(nwords, dtype = my_float_type_numpy)
        mylib.read_record_data(fd,
                               rec.contents.data_offset,
                               rec.contents.disk_length,
                               file_type.byte_ordering,
                               file_type.word_size,
                               rec.contents.int_hdr,
                               rec.contents.real_hdr,
                               nwords,
                               data)
        print data

