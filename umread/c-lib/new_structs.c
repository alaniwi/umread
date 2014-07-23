#include "umfileint.h"

/* functions to create pointers to new Rec, Var and File structures from heap memory
 *
 * Also for good measure functions to free these, although not crucial if they are not 
 * called, because of the garbage collection procedure employed elsewhere
 */

Rec *new_rec(int word_size, List *heaplist)
{
  Rec *rec;

  CKP(
      rec = malloc_(sizeof(Rec), heaplist)
      );
  CKP(
      rec->internp = malloc_(sizeof(struct _Rec), heaplist)
      );
  CKP(
      rec->int_hdr = malloc_(N_INT_HDR * word_size, heaplist)
      );
  CKP(
      rec->real_hdr = malloc_(N_INT_HDR * word_size, heaplist)
      );

  rec->header_offset = -1;
  rec->data_offset = -1;
  rec->disk_length = -1;
  return rec;

  ERRBLKP("new_rec");
}

int free_rec(Rec *rec, List *heaplist)
{
  CKI(   free_(rec->internp, heaplist)   );
  CKI(   free_(rec->int_hdr, heaplist)   );
  CKI(   free_(rec->real_hdr, heaplist)   );
  CKI(   free_(rec, heaplist)   );
  return 0;

  ERRBLKI("free_rec");
}

Var *new_var(List *heaplist)
{
  Var *var;
  CKP(
      var = malloc_(sizeof(Var), heaplist)
      );
  CKP(
      var->internp = malloc_(sizeof(struct _Var), heaplist)
      );
  var->nz = 0;
  var->nt = 0;
  var->supervar_index = -1;
  var->recs = NULL;
  return var;

  ERRBLKP("new_var");
}

int free_var(Var *var, List *heaplist)
{
  CKI(   free_(var->internp, heaplist)   );
  if (var->recs)
    CKI(   free_(var->recs, heaplist)   );
  CKI(   free_(var, heaplist)   );
  return 0;

  ERRBLKI("free_var");
}


/* new_file is a rather special case, because the heaplist is initialised as
 * part of the structure rather than supplied externally.  Also free_file will
 * free everything on the heaplist.
 */

File *new_file()
{
  File *file;

  if (  (file = malloc_(sizeof(File), NULL))  == NULL) goto err1;
  if (  (file->internp = malloc_(sizeof(struct _File), NULL))  == NULL) goto err2;
  if (  (file->internp->heaplist = list_new(NULL))  == NULL) goto err3;

  file->nvars = 0;
  file->vars = NULL;
  file->fd = -1;
  file->file_type.format = -1;
  file->file_type.byte_ordering = -1;
  file->file_type.word_size = -1;
  file->internp->nrec = 0;
  file->internp->recs = NULL;

  return file;
  
 err3:
  free_(file->internp, NULL);
 err2:
  free_(file, NULL);
 err1:
  error("new_file");
  return NULL;
}


int free_file(File *file)
{
  CKI(   free_all(file->internp->heaplist)   );
  CKI(   free_(file->internp, NULL)   );
  CKI(   free_(file, NULL)   );
  return 0;

  ERRBLKI("free_file");
}
