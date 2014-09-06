#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "umfileint.h"

// void *xmalloc(size_t size)
// {
//   void *ptr;
//   ptr = malloc(size);
//   if (ptr == NULL)
//     {
//       fprintf(stderr, "malloc %ld failed\n", size);
//       exit(1);
//     }
//   memset(ptr, 0, size);
//   printf("xmalloc(%d) -> %p\n", size, ptr);
//   return ptr;
// }
// 
// void xfree(void *ptr)
// {
//   free(ptr);
//   printf("xfree(%p)\n", ptr);
// }
// 
// File *file_alloc()
// {
//   File *file;
//   file = xmalloc(sizeof(File));
//   file->internp = xmalloc(sizeof(struct _File));
//   return file;
// }
// 
// Var *var_alloc()
// {
//   Var *var;
//   var = xmalloc(sizeof(Var));
//   var->internp = xmalloc(sizeof(struct _Var));
//   return var;
// }
// 
// 
// 
// Var *var_create_dummy(int k)
// {
//   int i;
//   int nrec;
//   Var *var;
//   var = var_alloc();
//   var->nz = k;
//   var->nt = k + 1;
//   var->supervar_index = 0;
//   var->internp->foo = 100 + k;
//   var->internp->blah = 200 + k;
//   nrec = var->nz * var->nt;
//   var->recs = xmalloc(nrec * sizeof(Rec *));
//   for (i = 0; i < nrec; i++)
//     var->recs[i] = rec_create_dummy_sgl(100 * k + i);
//   return var;
// }
// 
//File *file_create_dummy(File_type file_type)
//{  
//  File *file;
//  file = file_alloc();
//  file->file_type = file_type;
//  file->internp->foo = 3;
//  file->nvars = 2;
//  file->vars = xmalloc(2 * sizeof(Var *));
//  file->vars[0] = var_create_dummy(1);
//  file->vars[1] = var_create_dummy(2);
//  return file;
//}
//
/*===============================================*/
// 
// void _rec_internals_free(Rec *rec)
// {  
//   struct _Rec *internals;
//   internals = rec->internp;
//   if (!internals)
//     return;
//   /* add code for freeing any pointers hung off internals */
//   xfree(internals);
// }
// 
// 
// void _var_internals_free(Var *var)
// {  
//   struct _Var *internals;
//   internals = var->internp;
//   if (!internals)
//     return;
//   /* add code for freeing any pointers hung off internals */
//   xfree(internals);
// }
// 
// void var_free(Var *var)
// {
//   int recid, nrecs;
//   _var_internals_free(var);
//   if (var->recs)
//     {
//       nrecs = var->nz * var->nt;
//       if (nrecs > 0)
// 	for (recid = 0; recid < nrecs; recid++)
// 	  rec_free_sgl(var->recs[recid]);
//       xfree(var->recs);
//     }
//   xfree(var);
// }
// 
// void _file_internals_free(File *file)
// {
//   struct _File *internals;
//   internals = file->internp;
//   if (!internals)
//     return;
//   /* add code for freeing any pointers hung off internals */
//   xfree(internals);
// }
// 
//void file_free(File *file)
//{
//  int varid;
//  _file_internals_free(file);
//  if (file->vars)
//    {
//      if (file->nvars > 0)
//	for (varid = 0; varid < file->nvars; varid++)
//	  var_free(file->vars[varid]);
//      xfree(file->vars);
//    }
//  xfree(file);
//}
//
/*===============================================*/
// 
// void populate_file_type_dummy(File_type *ft)
// {
//   ft->format = plain_pp;
//   ft->byte_ordering = little_endian;
//   ft->word_size = 4;
// }
// 
// 		   
// int detect_file_type(int fd, File_type *file_type_rtn)
// {
//   populate_file_type_dummy(file_type_rtn);
//   return 0;
// }
// 

// void specify_file_type(File_format format,
// 		       Byte_ordering byte_ordering,
// 		       int word_size,
// 		       File_type *file_type_rtn)
// {
//   populate_file_type_dummy(file_type_rtn);
// }
// 
//
//File *file_parse(int fd,
//		 File_type file_type)
//{
//  return file_create_dummy(file_type);
//}
//


// int get_type_and_length(int word_size,
// 			const void *int_hdr,
// 			Data_type *type_rtn,
// 			size_t *num_words_rtn)
// {
//   return get_type_and_length_dummy_sgl(int_hdr, 
// 				       type_rtn,
// 				       num_words_rtn);
// }
// 

//int read_record_data(int fd, 
//		     size_t data_offset, 
//		     size_t disk_length, 
//		     Byte_ordering byte_ordering, 
//		     int word_size, 
//		     const void *int_hdr,
//		     const void *real_hdr,
//		     size_t nwords, 
//		     void *data_return)
//{
//  return read_record_data_core_sgl(fd, data_offset, disk_length, byte_ordering, word_size,
//				   int_hdr, real_hdr, nwords, data_return);
//}
//
//
// void read_header_dummy(size_t header_offset, 
// 		       void *int_hdr_rtn,
// 		       void *real_hdr_rtn) 
// {
//   int4 *ihdr;
//   real4 *rhdr;
//   int i;
//   ihdr = int_hdr_rtn;
//   rhdr = real_hdr_rtn;
//   for (i = 0; i < 45; i++)
//     ihdr[i] = i + 1000 + header_offset;
//   for (i = 0; i < 19; i++)
//     rhdr[i] = i + 1000.1 + header_offset;
// } 
// 
// int read_header(int fd,
// 		size_t header_offset,
// 		Byte_ordering byte_ordering, 
// 		int word_size, 
// 		void *int_hdr_rtn,
// 		void *real_hdr_rtn) 
// {
//   assert(byte_ordering == little_endian);
//   assert(word_size == 4);
// 
//   read_header_dummy(header_offset, int_hdr_rtn, real_hdr_rtn);
//   return 0;
// }
// 
#ifdef MAIN
int main()
{
  return main_sgl();
}
#endif
