#include <stddef.h>

typedef enum {
  plain_pp,
  fields_file
} File_format;

typedef enum {
  little_endian,
  big_endian
} Byte_ordering;

struct _File;
struct _Var;
struct _Rec;

typedef struct {
  File_format format;
  Byte_ordering byte_ordering;
  int word_size;
} File_type ;

typedef struct {
  void *int_hdr;
  void *real_hdr;
  size_t header_offset;  /* in words */
  size_t data_offset;  /* in words */
  struct _Rec *internp;
} Rec;

typedef struct {
  Rec **recs;
  int nz;
  int nt;
  int supervar_index;
  struct _Var *internp;
} Var;

typedef struct {
  int fd;
  File_type file_type;
  int nvars;
  Var **vars;
  struct _File *internp;
} File;

/* ------------------------------------------------------------------- */

int detect_file_type(int fd, File_type *file_type_rtn);
/* 
   Given an open file, detect type of file (caller provides storage for info returned).
   if detection was successful, returns 0 and populates the File_type structure provided 
   by the caller, otherwise returns 1.   
*/

// void specify_file_type(File_format format,
// 		       Byte_ordering byte_ordering,
// 		       int word_size,
// 		       File_type *file_type_rtn);
// /* 
//    Populates the File_type structure using the other arguments passed.
// */
// 

File *file_parse(int fd,
		 File_type file_type);
/* 
   Given an open file handle, parse a file into a File structure, with embedded 
   Var and Rec structures, relating the PP records to variables within the file.

   Caller should pass a File_type structure as either returned from
   detect_file_type() or populated by the caller.
*/

/* commented out - to handle in Python */
/* void close_fd(File *file); */
/* int reopen_fd(File *file); */

/* ------------------------------------------------------------------- */
/* functions for reading the actual data, not dependent on the above objects 
 * (although may call common code in the implementation)
 */

void read_header(int fd,
		 size_t header_offset,
		 Byte_ordering byte_ordering, 
		 int word_size, 
		 void *int_hdr_rtn,
		 void *real_hdr_rtn);
/*
  reads a PP header at specified offset; function will do byte-swapping
  as necessary, but returned header data will match word size, and 
  caller must provide storage of appropriate length to contain these
  (45 and 19 words respectively)
*/


size_t get_nwords(int word_size,
		  const void *int_hdr);
/*
  Parses integer PP header, returning number of data words.  Caller provides
  integer header as array of 4 or 8 byte ints as appropriate to passed
  word_size
 */

int read_record_data(int fd, 
		     size_t data_offset, 
		     Byte_ordering byte_ordering, 
		     int word_size, 
		     const void *int_hdr,
		     const void *real_hdr,
		     size_t nwords, 
		     void *data_return);
/* 
   Reads record data at specified offset; function will do byte-swapping and
   unpacking as necessary.  Caller provides PP headers as arrays of 4 or 8
   byte ints and floats/doubles as appropriate to passed word_size (real
   header needed for missing data value).  This must match actual file word
   size, and there will be no casting of data except as appropriate when
   unpacking packed fields.

   Returns in data_return an array of int or float at this word size; caller 
   must provide storage of size nwords words, and nwords must have been
   obtained by calling get_nwords().

   Return value is 0 for success, 1 for failure.
*/
/* ------------------------------------------------------------------- */
