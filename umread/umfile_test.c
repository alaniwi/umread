#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "umfile.h"
#include "umfileint.h"

typedef int myint;
typedef float myfloat;

void *xmalloc(size_t size)
{
  void *ptr;
  ptr = malloc(size);
  if (ptr == NULL)
    {
      fprintf(stderr, "malloc %ld failed\n", size);
      exit(1);
    }
  return ptr;
}

File *file_alloc()
{
  File *file;
  file = xmalloc(sizeof(File));
  file->internp = xmalloc(sizeof(struct _File));
  return file;
}

Var *var_alloc()
{
  Var *var;
  var = xmalloc(sizeof(Var));
  var->internp = xmalloc(sizeof(struct _Var));
  return var;
}

Rec *rec_alloc()
{
  Rec *rec;
  rec = xmalloc(sizeof(Rec));
  rec->internp = xmalloc(sizeof(struct _Rec));
  rec->int_hdr = xmalloc(45 * sizeof(myint));
  rec->real_hdr = xmalloc(19 * sizeof(myfloat));
  return rec;
}

Rec *rec_create_dummy(int k)
{
  int i;
  Rec *rec;
  rec = rec_alloc();
  for (i = 0; i < 45 ; i++)
    ((myint *)rec->int_hdr)[i] = k * 100 + i;
  for (i = 0; i < 19 ; i++)
    ((myfloat *)rec->real_hdr)[i] = k + i / 100.;
  rec->header_offset = k;
  rec->data_offset = 500 + k;
  rec->internp->blahblah = 200 + k;
  return rec;
}

Var *var_create_dummy(int k)
{
  int i;
  int nrec;
  Var *var;
  var = var_alloc();
  var->nz = k;
  var->nt = k + 1;
  var->supervar_index = 0;
  var->internp->foo = 100 + k;
  var->internp->blah = 200 + k;
  nrec = var->nz * var->nt;
  var->recs = xmalloc(nrec * sizeof(Rec *));
  for (i = 0; i < nrec; i++)
    var->recs[i] = rec_create_dummy(100 * k + i);
  return var;
}

File *file_create_dummy(File_type file_type)
{  
  File *file;
  file = file_alloc();
  file->file_type = file_type;
  file->internp->foo = 3;
  file->nvars = 2;
  file->vars = xmalloc(2 * sizeof(Var *));
  file->vars[0] = var_create_dummy(1);
  file->vars[1] = var_create_dummy(2);
  return file;
}


void populate_file_type_dummy(File_type *ft)
{
  ft->format = plain_pp;
  ft->byte_ordering = little_endian;
  ft->word_size = 4;
}
		   
int detect_file_type(int fd, File_type *file_type_rtn)
{
  populate_file_type_dummy(file_type_rtn);
  return 0;
}

void specify_file_type(File_format format,
		       Byte_ordering byte_ordering,
		       int word_size,
		       File_type *file_type_rtn)
{
  populate_file_type_dummy(file_type_rtn);
}


File *file_parse(int fd,
		 File_type file_type)
{
  return file_create_dummy(file_type);
}


size_t get_type_and_length_dummy(const void *int_hdr, Data_type *type_rtn, size_t *num_words_rtn)
{
  const myint *int_hdr_4 = int_hdr;
  *num_words_rtn = int_hdr_4[0];
  *type_rtn = real_type;
}

int get_type_and_length(int word_size,
			const void *int_hdr,
			Data_type *type_rtn,
			size_t *num_words_rtn)
{
  return get_type_and_length_dummy(int_hdr, 
				   type_rtn,
				   num_words_rtn);
}



void read_record_data_dummy(size_t nwords, 
			    void *data_return)
{
  int i;
  myfloat *data_return_4 = data_return;
  for (i = 0; i < nwords; i++) {
    data_return_4[i] = i / 100.;    
  }
} 

int read_record_data(int fd, 
		     size_t data_offset, 
		     Byte_ordering byte_ordering, 
		     int word_size, 
		     const void *int_hdr,
		     const void *real_hdr,
		     size_t nwords, 
		     void *data_return)
{
  int i;
  assert(byte_ordering == little_endian);
  assert(word_size == 4);

  printf("start of int header seen in read_record_data_dummy():");
  for (i = 0; i < 5; i++)
    printf("  %d", ((int4 *) int_hdr)[i]);
  printf("\n");
  printf("start of int header seen in read_record_data_dummy():");
  for (i = 0; i < 5; i++)
    printf("  %f", ((real4 *) real_hdr)[i]);
  printf("\n");

  read_record_data_dummy(nwords, data_return);
  return 0;
}


void read_header_dummy(size_t header_offset, 
		       void *int_hdr_rtn,
		       void *real_hdr_rtn) 
{
  int4 *ihdr;
  real4 *rhdr;
  int i;
  ihdr = int_hdr_rtn;
  rhdr = real_hdr_rtn;
  for (i = 0; i < 45; i++)
    ihdr[i] = i + 1000 + header_offset;
  for (i = 0; i < 19; i++)
    rhdr[i] = i + 1000.1 + header_offset;
} 

int read_header(int fd,
		size_t header_offset,
		Byte_ordering byte_ordering, 
		int word_size, 
		void *int_hdr_rtn,
		void *real_hdr_rtn) 
{
  assert(byte_ordering == little_endian);
  assert(word_size == 4);

  read_header_dummy(header_offset, int_hdr_rtn, real_hdr_rtn);
  return 0;
}

#ifdef MAIN
int main()
{
  int i, j, k, nrec;
  int fd;
  File *file;
  File_type file_type;
  Var *var;
  Rec *rec;
  myfloat *data;
  int word_size;
  Data_type data_type;
  size_t nwords, nbytes;
 
  fd = 3;
  detect_file_type(fd, &file_type);
  printf("word size = %d\n", file_type.word_size);
  
  file = file_parse(fd, file_type);
  for (i = 0; i < file->nvars; i++)
    {
      printf("var %d\n", i);
      var = file->vars[i];
      printf("nz = %d, nt = %d\n", var->nz, var->nt);
      nrec = var->nz * var->nt;
      for (j = 0; j < nrec; j++)
	{
	  rec = var->recs[j];
	  printf("var %d rec %d\n", i, j);
	  printf("int header:\n");
	  for (k = 0; k < 45; k++)
	    printf(" ihdr[%d] = %d\n", k, ((myint *)rec->int_hdr)[k]);
	  printf("real header:\n");
	  for (k = 0; k < 19; k++)
	    printf(" rhdr[%d] = %f\n", k, ((myfloat *)rec->real_hdr)[k]);

	  word_size = file_type.word_size;
	  get_type_and_length(word_size, rec->int_hdr, &data_type, &nwords);
	  nbytes = word_size * nwords;;
	  printf("data (%ld items)\n", nwords);
	  data = xmalloc(nbytes * sizeof(float));
	  read_record_data(fd, 
			   rec->data_offset,
			   file_type.byte_ordering,
			   file_type.word_size,
			   rec->int_hdr,
			   rec->real_hdr,
			   nwords,
			   data);
	  for (k = 0; k < nwords; k++)
	    printf(" data[%d] = %f\n", k, data[k]);
	  free(data);	  
	}
    }
  return 0;
}
#endif
