#include "datatype.h"

// FIXME - remove test protos------------------
Rec *WITH_LEN(rec_alloc)();
void WITH_LEN(rec_free)(Rec *rec);
Rec *WITH_LEN(rec_create_dummy)(int k);
int WITH_LEN(get_type_and_length_dummy)(const void *int_hdr, Data_type *type_rtn, size_t *num_words_rtn);
void WITH_LEN(read_record_data_dummy)(size_t nwords, 
				       void *data_return);
int WITH_LEN(read_record_data)(int fd, 
			       size_t data_offset, 
			       size_t disk_length, 
			       Byte_ordering byte_ordering, 
			       int word_size, 
			       const void *int_hdr,
			       const void *real_hdr,
			       size_t nwords, 
			       void *data_return);
//-----------------------

/* interpret_header.c */
Data_type WITH_LEN(get_type)(const INTEGER *int_hdr);
size_t WITH_LEN(get_data_length) (const INTEGER *int_hdr);

int WITH_LEN(get_type_and_length)(const INTEGER *int_hdr,
				  Data_type *type_rtn,
				  size_t *num_words_rtn);

/* read.c */
void WITH_LEN(swap_bytes)(void *ptr, size_t num_words);
void WITH_LEN(swapbytes_if_swapped)(void *ptr, 
				    size_t num_words,
				    Byte_ordering byte_ordering);
size_t WITH_LEN(read_words)(int fd, 
			    void *ptr,
			    size_t num_words,
			    Byte_ordering byte_ordering);
int WITH_LEN(read_header_at_offset)(int fd,
				    size_t header_offset,
				    Byte_ordering byte_ordering, 
				    INTEGER *int_hdr_rtn,
				    REAL *real_hdr_rtn);
int WITH_LEN(read_header)(int fd,
			  Byte_ordering byte_ordering, 
			  INTEGER *int_hdr_rtn,
			  REAL *real_hdr_rtn);
Rec *WITH_LEN(get_record)(File *file, List *heaplist);
int WITH_LEN(read_all_headers)(File *file, List *heaplist);
size_t WITH_LEN(skip_fortran_record)(File *file);
int WITH_LEN(skip_word)(File *file);
int WITH_LEN(read_all_headers_pp)(File *file, List *heaplist);
int WITH_LEN(read_all_headers_ff)(File *file, List *heaplist);
int WITH_LEN(get_ff_disk_length)(INTEGER *ihdr);
int WITH_LEN(get_valid_records_ff)(int fd,
				   Byte_ordering byte_ordering,
				   size_t hdr_start, size_t hdr_size, int nrec,
				   int valid[], int *n_valid_rec_return);


/* process_vars.c */
int WITH_LEN(process_vars)(File *file, List *heaplist);
int WITH_LEN(lev_set)(Level *lev, Rec *rec);

/* date_and_time.c */
REAL WITH_LEN(mean_period)(const Time *time);
int WITH_LEN(is_time_mean)(INTEGER LBTIM);
REAL WITH_LEN(time_diff)(INTEGER lbtim, const Date *date, const Date *orig_date);
REAL WITH_LEN(sec_to_day)(int8 seconds);
Calendar_type WITH_LEN(calendar_type)(INTEGER type);
int8 WITH_LEN(gregorian_to_secs)(const Date *date);
int WITH_LEN(time_set)(Time *time, Rec *rec);


/* Debug_dump.c */
void WITH_LEN(debug_dump_all_headers)(File *file);

#ifdef MAIN
int WITH_LEN(main)();
#endif
