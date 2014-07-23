/* PROTOTYPES */

/* error.c */
void switch_bug(const char *routine);
void error(const char *routine);
void error_mesg(const char *routine, const char *fmt, ...);
void debug(const char *fmt, ...);
void errorhandle_init();

/* malloc.c */
void *malloc_(size_t size, List *heaplist);
void *dup_(const void *inptr, size_t size, List *heaplist);
int free_(void *ptr, List *heaplist);
int free_all(List *heaplist);

/* linklist.c */
typedef int(*free_func) (void *, List *);

void *list_new(List *heaplist);
int list_free(List *list, int free_ptrs, List *heaplist);
int list_size(const List *list);
int list_add(List *list, void *ptr, List *heaplist);
int list_add_or_find(List *list, 
		     void *item_in,
		     int (*compar)(const void *, const void *), 
		     int matchval, 
		     free_func free_function,
		     int *index_return,
		     List *heaplist);
int list_del(List *list, void *ptr, List *heaplist);
int list_del_by_listel(List *list, List_element *p, List *heaplist);
int list_startwalk(const List *list, List_handle *handle);
void *list_walk(List_handle *handle, int return_listel);
void *list_find(List *list,
		const void *item,
		int (*compar)(const void *, const void *), 
		int matchval,
		int *index_return);

/* filetype.c */
int detect_file_type_(int fd, File_type *file_type);

/* new_structs.c */
Rec *new_rec(int word_size, List *heaplist);
int free_rec(Rec *rec, List *heaplist);
Var *new_var(List *heaplist);
File *new_file();
int free_file(File *file);

