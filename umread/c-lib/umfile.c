#include <stdio.h>
#include <unistd.h>

#include "umfileint.h"

int get_type_and_length(int word_size,
			const void *int_hdr,
			Data_type *type_rtn,
			size_t *num_words_rtn)
{
  errorhandle_init();
  switch (word_size)
    {
    case 4:
      return get_type_and_length_sgl(int_hdr, type_rtn, num_words_rtn);
    case 8:
      return get_type_and_length_dbl(int_hdr, type_rtn, num_words_rtn);
    default:
      return -1;
    }
}

int detect_file_type(int fd, File_type *file_type)
{
  errorhandle_init();
  return detect_file_type_(fd, file_type);
}


int read_header(int fd,
		size_t header_offset,
		Byte_ordering byte_ordering, 
		int word_size, 
		void *int_hdr_rtn,
		void *real_hdr_rtn)
{
  errorhandle_init();
  switch (word_size)
    {
    case 4:
      return read_header_at_offset_sgl(fd, header_offset, byte_ordering, 
				       int_hdr_rtn, real_hdr_rtn);
    case 8:
      return read_header_at_offset_dbl(fd, header_offset, byte_ordering, 
				       int_hdr_rtn, real_hdr_rtn);
    default:
      return -1;
    }
}


File *file_parse(int fd,
		 File_type file_type)
{
  File *file;
  List *heaplist;

  errorhandle_init();

  CKP(  file = new_file()  );
  file->fd = fd;
  file->file_type = file_type;

  heaplist = file->internp->heaplist;

  switch (file_type.word_size)
    {
    case 4:
      CKI(  read_all_headers_sgl(file, heaplist)  );
      debug_dump_all_headers_sgl(file);
      CKI(  process_vars_sgl(file, heaplist)  );
      break;
    case 8:
      CKI(  read_all_headers_dbl(file, heaplist)  );
      debug_dump_all_headers_dbl(file);
      CKI(  process_vars_dbl(file, heaplist)  );
      break;
    default:
      ERR;
    }
  return file;

 err:
  if (file)
    free_file(file);
  return NULL;
}


void file_free(File *file)
{
  errorhandle_init();

  CKI(   free_file(file)   );
  return;

 err:
  error("free_file");
}
