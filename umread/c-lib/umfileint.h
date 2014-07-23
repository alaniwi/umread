#include <inttypes.h>
#include <endian.h>

#include "umfile.h"

typedef int32_t int4;
typedef int64_t int8;
typedef float real4;
typedef double real8;

/*---------------------------*/
/* for linked list */

struct _list_element
{
  void *ptr;
  struct _list_element *prev;
  struct _list_element *next;
};
typedef struct _list_element List_element;

typedef struct 
{
  int n;
  List_element *first;
  List_element *last;
}
  List;

typedef struct 
{
  /* This is a little structure which stores the information needed for
   * pp_list_walk.  Its main purpose is to store the position outside the list
   * structure itself, so that for read-only scanning of the list, the PPlist*
   * can be declared as const.
   */
  List_element *current;
  const List *list;
}
  List_handle;

/*---------------------------*/


struct _File
{
  int foo; //FIXME
  List *heaplist;
  int nrec;
  Rec **recs;
};

struct _Var 
{
  int foo; //FIXME
  int blah; //FIXME
};

struct _Rec 
{
  //  Level *lev;
  //  Time *time;
  int zindex; /* index on z axis within a variable - used for detecting vars with irreg z,t */
  int tindex; /* index on t axis within a variable - used for detecting vars with irreg z,t */
  int disambig_index; /* index used for splitting variables with irreg z,t into 
                       * sets of variables with regular z,t */
  int supervar_index; /* when a variable is split, this is set to an index which is common
                       * across the set, but different from sets generated from other
                       * super-variables
                       */
  real8 mean_period; /* period (in days) of time mean 
                        (store here so as to calculate once only) */  
};

/*---------------------------*/

typedef enum {single_precision, double_precision} data_type;

#if BYTE_ORDER == LITTLE_ENDIAN
#define NATIVE_ORDERING little_endian
#define REVERSE_ORDERING big_endian
#elif BYTE_ORDER == BIG_ENDIAN
#define NATIVE_ORDERING big_endian
#define REVERSE_ORDERING little_endian
#else
#error BYTE_ORDER not defined properly
#endif

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


//FIXME: remove test prototypes

void *xmalloc(size_t size);
void xfree(void *ptr);
File *file_alloc();
Var *var_alloc();
Var *var_create_dummy(int k);
File *file_create_dummy(File_type file_type);
void _rec_internals_free(Rec *rec);
void _var_internals_free(Var *var);
void var_free(Var *var);
void _file_internals_free(File *file);
void populate_file_type_dummy(File_type *ft);
void read_header_dummy(size_t header_offset, 
		       void *int_hdr_rtn,
		       void *real_hdr_rtn);

/* ----------------------------------------------------------- */
/* PP header interpretation */

#define N_INT_HDR 45
#define N_REAL_HDR 19
#define N_HDR (N_INT_HDR + N_REAL_HDR)

#define INDEX_LBYR    0
#define INDEX_LBMON   1
#define INDEX_LBDAT   2
#define INDEX_LBHR    3
#define INDEX_LBMIN   4
#define INDEX_LBDAY   5
#define INDEX_LBYRD   6
#define INDEX_LBMOND  7
#define INDEX_LBDATD  8
#define INDEX_LBHRD   9
#define INDEX_LBMIND  10
#define INDEX_LBDAYD  11
#define INDEX_LBTIM   12
#define INDEX_LBFT    13
#define INDEX_LBLREC  14
#define INDEX_LBCODE  15
#define INDEX_LBHEM   16
#define INDEX_LBROW   17
#define INDEX_LBNPT   18
#define INDEX_LBEXT   19
#define INDEX_LBPACK  20
#define INDEX_LBREL   21
#define INDEX_LBFC    22
#define INDEX_LBCFC   23
#define INDEX_LBPROC  24
#define INDEX_LBVC    25
#define INDEX_LBRVC   26
#define INDEX_LBEXP   27
#define INDEX_LBBEGIN 28
#define INDEX_LBNREC  29
#define INDEX_LBPROJ  30
#define INDEX_LBTYP   31
#define INDEX_LBLEV   32
#define INDEX_LBRSVD1 33
#define INDEX_LBRSVD2 34
#define INDEX_LBRSVD3 35
#define INDEX_LBRSVD4 36
#define INDEX_LBSRCE  37
#define INDEX_LBUSER1 38
#define INDEX_LBUSER2 39
#define INDEX_LBUSER3 40
#define INDEX_LBUSER4 41
#define INDEX_LBUSER5 42
#define INDEX_LBUSER6 43
#define INDEX_LBUSER7 44
#define INDEX_BULEV    0
#define INDEX_BHULEV   1
#define INDEX_BRSVD3   2
#define INDEX_BRSVD4   3
#define INDEX_BDATUM   4
#define INDEX_BACC     5
#define INDEX_BLEV     6
#define INDEX_BRLEV    7
#define INDEX_BHLEV    8
#define INDEX_BHRLEV   9
#define INDEX_BPLAT   10
#define INDEX_BPLON   11
#define INDEX_BGOR    12
#define INDEX_BZY     13
#define INDEX_BDY     14
#define INDEX_BZX     15
#define INDEX_BDX     16
#define INDEX_BMDI    17
#define INDEX_BMKS    18

#define LOOKUP(rec, index) (((INTEGER *)((rec)->int_hdr))[index])
#define RLOOKUP(rec, index) (((REAL *)((rec)->real_hdr))[index])

/* ----------------------------------------------------------- */

/* error-checking macros */

/* these are to allow a compact way of incorporating error-checking of 
 * the return value of a function call, without obfuscating the basic purpose
 * of the line of code, which is executing the function call.
 *
 * CKI used for integer functions which return negative value on failure
 * CKP used for pointer functions which return NULL on failure
 * CKF for floats for good measure (probably not used)
 *
 * put the ERRBLK (or ERRBLKI or ERRBLKP) at the end of the subroutine, with 
 * the "label" argument set to the subroutine name (as a string)
 */

#define FLT_ERR -1e38

#ifdef DEBUG
#define ERR abort();
#else
/* ERR: unconditional branch */
#define ERR goto err;
#endif

#define CKI(i) if ((i) < 0){ ERR }
#define CKP(p) if ((p) == NULL){ ERR }
#define CKF(f) if ((f) == FLT_ERR){ ERR }

/* ERRIF: conditional branch */
#define ERRIF(i) if (i){ ERR }

#define ERRBLK(label, rtn) err: error(label); return (rtn);
#define ERRBLKI(label) ERRBLK((label), -1);
#define ERRBLKP(label) ERRBLK((label), NULL);
#define ERRBLKF(label) ERRBLK((label), FLT_ERR);

/* ----------------------------------------------------------- */


/* prototypes for datatype dependent functions are included via the following 
 * files automatically generated from type_dep_protos.h */

#include "protos_sgl.h"
#include "protos_dbl.h"
