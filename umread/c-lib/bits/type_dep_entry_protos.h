/* prototypes for entry points to the type-dependent code;
 * need to be usable without INTEGER, etc, being defined, 
 * so void* used for some pointer types that have specific
 * pointer types defined in the function declarations themselves
 */

void WITH_LEN(swap_bytes)(void *ptr, size_t num_words);

int WITH_LEN(get_type_and_length_core)(const void *int_hdr,
				       Data_type *type_rtn,
				       size_t *num_words_rtn);

int WITH_LEN(read_hdr_at_offset)(int fd,
				 size_t header_offset,
				 Byte_ordering byte_ordering, 
				 void *int_hdr_rtn,
				 void *real_hdr_rtn);

int WITH_LEN(read_record_data_core)(int fd, 
				    size_t data_offset, 
				    size_t disk_length, 
				    Byte_ordering byte_ordering, 
				    int word_size, 
				    const void *int_hdr,
				    const void *real_hdr,
				    size_t nwords, 
				    void *data_return);

File *WITH_LEN(file_parse_core)(int fd,
				File_type file_type);
