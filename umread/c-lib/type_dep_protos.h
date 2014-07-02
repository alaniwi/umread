#include "datatype.h"

Rec *WITH_TYPE(rec_alloc)();
void WITH_TYPE(rec_free)(Rec *rec);
Rec *WITH_TYPE(rec_create_dummy)(int k);
int WITH_TYPE(get_type_and_length_dummy)(const void *int_hdr, Data_type *type_rtn, size_t *num_words_rtn);
void WITH_TYPE(read_record_data_dummy)(size_t nwords, 
				       void *data_return);
int WITH_TYPE(read_record_data)(int fd, 
				size_t data_offset, 
				Byte_ordering byte_ordering, 
				int word_size, 
				const void *int_hdr,
				const void *real_hdr,
				size_t nwords, 
				void *data_return);

#ifdef MAIN
int WITH_TYPE(main)();
#endif
