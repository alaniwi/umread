#include <inttypes.h>

struct _File {
  int foo;
};

struct _Var {
  int foo;
  int blah;
};

struct _Rec {
  int blahblah;
};

typedef int32_t int4;
typedef int64_t int8;
typedef float real4;
typedef double real8;

typedef enum {single_precision, double_precision} data_type;


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

/* prototypes for datatype dependent functions are included via the following 
 * files automatically generated from type_dep_protos.h */

#include "protos_sgl.h"
#include "protos_dbl.h"
