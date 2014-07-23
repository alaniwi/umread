#include <unistd.h>

#include "umfileint.h"
#include "datatype.h"


#define file_pos(f) (lseek(f, 0, SEEK_CUR))

void WITH_LEN(swap_bytes)(void *ptr, size_t num_words)
{
  int i;
  char *p;
  char t;

  p = (char*) ptr;
  for (i = 0; i < num_words; i++)
    {
#define DO_SWAP(x, y) {t = p[x]; p[x] = p[y]; p[y] = t;}
#if defined(SINGLE)
      DO_SWAP(3, 0);
      DO_SWAP(2, 1);
      p += 4;  
#elif defined(DOUBLE)
      DO_SWAP(7, 0);
      DO_SWAP(6, 1);
      DO_SWAP(5, 2);
      DO_SWAP(4, 3);
      p += 8;      
#else
#error Need to compile this file with -DSINGLE or -DDOUBLE
#endif
    }
}

void WITH_LEN(swapbytes_if_swapped)(void *ptr, 
				    size_t num_words,
				    Byte_ordering byte_ordering)
{
  if (byte_ordering == REVERSE_ORDERING)
    WITH_LEN(swap_bytes)(ptr, num_words);
}


/*
 * reads n words from file, storing them at ptr, with byte swapping as required
 * returns number of words read (i.e. n, unless there's a short read)
 */
size_t WITH_LEN(read_words)(int fd, 
			    void *ptr,
			    size_t num_words,
			    Byte_ordering byte_ordering)
{
  size_t nread;

  CKP(ptr);
  nread = read(fd, ptr, num_words * WORD_SIZE) / WORD_SIZE;
  WITH_LEN(swapbytes_if_swapped)(ptr, nread, byte_ordering);
  return nread;

  ERRBLKI("read_words");
}


int WITH_LEN(read_header_at_offset)(int fd,
				    size_t header_offset,
				    Byte_ordering byte_ordering, 
				    INTEGER *int_hdr_rtn,
				    REAL *real_hdr_rtn)
{
  /* as read_header below, but also specifying the file offset in bytes */

  CKI(  lseek(fd, header_offset, SEEK_SET)  );
  return WITH_LEN(read_header)(fd, byte_ordering, int_hdr_rtn, real_hdr_rtn);

  ERRBLKI("read_header_at_offset");
}


int WITH_LEN(read_header)(int fd,
			  Byte_ordering byte_ordering, 
			  INTEGER *int_hdr_rtn,
			  REAL *real_hdr_rtn)
{
  /* reads a PP header at specified word offset into storage 
     provided by the caller */

  ERRIF(   WITH_LEN(read_words)(fd, int_hdr_rtn, 
				N_INT_HDR, byte_ordering)   != N_INT_HDR);

  ERRIF(   WITH_LEN(read_words)(fd, real_hdr_rtn, 
				N_REAL_HDR, byte_ordering)   != N_REAL_HDR);

  return 0;

  ERRBLKI("read_header");    
}


Rec *WITH_LEN(get_record)(File *file, List *heaplist)
{
  /* reads PP headers and returns a Rec structure -- 
   *
   * file must be positioned at start of header (after any fortran record length integer) on entry,
   * and will be positioned at end of header on return
   *
   * the Rec structure will contain the headers in elements int_hdr and real_hdr, 
   * but other elements will be left as initialised by new_rec()
   */

  Rec *rec;

  CKP(   rec = new_rec(WORD_SIZE, heaplist)   );

  CKI(
      WITH_LEN(read_header)(file->fd,
			    file->file_type.byte_ordering,
			    rec->int_hdr,
			    rec->real_hdr)
      );
  
  return rec; /* success */

  ERRBLKP("get_record");    
}

int WITH_LEN(read_all_headers)(File *file, List *heaplist)
{
  switch (file->file_type.format)
    {
    case plain_pp:
      return WITH_LEN(read_all_headers_pp)(file, heaplist);
    case fields_file:
      return WITH_LEN(read_all_headers_ff)(file, heaplist);
    default:
      switch_bug("read_all_headers");
      ERR;
    }
  return 0;
  
  ERRBLKI("read_all_headers");
}

/* skip_fortran_record: skips a fortran record, and returns how big it was,
 *  or -1 for end of file, or -2 for any error which may imply corrupt file
 * (return value of 0 is a legitimate empty record).
 */
size_t WITH_LEN(skip_fortran_record)(File *file)
{
  INTEGER rec_bytes, rec_bytes_2;

  if(   WITH_LEN(read_words)(file->fd, &rec_bytes, 1, 
			     file->file_type.byte_ordering)   != 1) return -1;
  CKI(   lseek(file->fd, rec_bytes, SEEK_CUR)   );
  ERRIF(   WITH_LEN(read_words)(file->fd, &rec_bytes_2, 1, 
				file->file_type.byte_ordering)   != 1);
  ERRIF(rec_bytes != rec_bytes_2);
  return rec_bytes;
  
  ERRBLK("skip_fortran_record", -2);
}

int WITH_LEN(skip_word)(File *file)
{
  CKI(   lseek(file->fd, WORD_SIZE, SEEK_CUR)   );
  return 0;

  ERRBLKI("skip_word");
}

int WITH_LEN(read_all_headers_pp)(File *file, List *heaplist)
{
  int fd;
  size_t nrec, rec_bytes, recno, header_offset;
  Rec **recs, *rec;

  fd = file->fd;

  /* count the PP records in the file */
  lseek(fd, 0, SEEK_SET);
  for (nrec = 0; (rec_bytes = WITH_LEN(skip_fortran_record)(file)) != -1; nrec++) 
    {
      ERRIF(rec_bytes == -2);
      if (rec_bytes != N_HDR * WORD_SIZE) {
	error_mesg("read_all_headers_pp", 
		   "unsupported header length in PP file: %d words", rec_bytes / WORD_SIZE);
	ERR;
      }
      ERRIF(   WITH_LEN(skip_fortran_record)(file)   < 0); /* skip the data record */
    }
  
  /* now rewind, and read in all the PP header data */
  CKP(   recs = malloc_(nrec * sizeof(Rec *), heaplist)   );
  file->internp->nrec = nrec;
  file->internp->recs = recs;

  lseek(fd, 0, SEEK_SET);
  for (recno = 0; recno < nrec; recno++)
    {
      CKI(   WITH_LEN(skip_word)(file)   );
      header_offset = file_pos(fd);
      CKP(   rec = WITH_LEN(get_record)(file, heaplist)   );
      CKI(   WITH_LEN(skip_word)(file)   );
      recs[recno] = rec;

      /* skip data record but store length */
      rec->header_offset = header_offset;
      rec->data_offset = file_pos(fd) + WORD_SIZE;
      rec->disk_length = WITH_LEN(skip_fortran_record)(file);
    }
  return 0;

  ERRBLKI("read_all_headers_pp");
}


#define READ_ITEM(x) \
  ERRIF(   WITH_LEN(read_words)(fd, &x, 1, byte_ordering)   != 1);

int WITH_LEN(read_all_headers_ff)(File *file, List *heaplist)
{
  int fd;
  int is_fields_file;
  size_t hdr_start, hdr_size, header_offset, data_offset_calculated, data_offset_specified;
  int *valid, n_valid_rec, n_raw_rec, i_valid_rec, i_raw_rec;
  Byte_ordering byte_ordering;
  Rec *rec, **recs;
  INTEGER start_lookup, nlookup1, nlookup2, dataset_type, start_data;

  fd = file->fd;
  byte_ordering = file->file_type.byte_ordering;
  
  /* pick out certain information from the fixed length header */    
  CKI(   lseek(fd, 4 * WORD_SIZE, SEEK_SET)  );
  READ_ITEM(dataset_type);

  ERRIF(   WITH_LEN(read_words)(fd, &dataset_type, 1, byte_ordering)   != 1);
  
  CKI(   lseek(fd, 149 * WORD_SIZE, SEEK_SET)  );
  READ_ITEM(start_lookup);
  READ_ITEM(nlookup1);
  READ_ITEM(nlookup2);

  CKI(   lseek(fd, 159 * WORD_SIZE, SEEK_SET)  );
  READ_ITEM(start_data);

  /* fieldsfiles includes ancillary files and initial dumps */
  is_fields_file = (dataset_type == 1 || dataset_type == 3 || dataset_type == 4);

  /* (first dim of lookup documented as being 64 or 128, so 
   * allow header longer than n_hdr (64) -- discarding excess -- but not shorter)
   */

  if (nlookup1 < N_HDR)
    {
      error_mesg("read_all_headers_pp", 
		 "unsupported header length: %d words", nlookup1);
      ERR;
    }

  CKP(  valid = malloc_(nlookup2 * sizeof(int), heaplist)   );

  hdr_start = (start_lookup - 1) * WORD_SIZE;
  hdr_size = nlookup1 * WORD_SIZE;
  n_raw_rec = nlookup2;
  CKI(  WITH_LEN(get_valid_records_ff)(fd, byte_ordering, hdr_start, hdr_size, n_raw_rec,
				       valid, &n_valid_rec)  );  

  /* now read in all the PP header data */
  
  CKP(   recs = malloc_(n_valid_rec * sizeof(Rec *), heaplist)   );
  debug("n_raw_rec=%d n_valid_rec=%d", n_raw_rec, n_valid_rec);
  file->internp->nrec = n_valid_rec;
  file->internp->recs = recs;
  
  i_valid_rec = 0;
  data_offset_calculated = (start_data - 1) * WORD_SIZE;
  for (i_raw_rec = 0; i_raw_rec < n_raw_rec; i_raw_rec++)
    {
      if (valid[i_raw_rec])
	{
	  header_offset = hdr_start + i_raw_rec * hdr_size;
	  CKI(   lseek(fd, header_offset, SEEK_SET)  );
	  CKP(   rec = WITH_LEN(get_record)(file, heaplist)   );
	  recs[i_valid_rec] = rec;
	  
	  rec->header_offset = header_offset;
	  rec->disk_length = WITH_LEN(get_ff_disk_length)(rec->int_hdr);
	  
	  data_offset_specified = (size_t) LOOKUP(rec, INDEX_LBBEGIN) * WORD_SIZE;
	  /* use LBBEGIN if available */
	  rec->data_offset =
	    (data_offset_specified != 0) ? data_offset_specified : data_offset_calculated;
	  
	  /* If LBNREC and LBBEGIN are both non-zero and it's not a FIELDSfile,
	   *   the file has well-formed records.  In that case, 
	   *   LBBEGIN should be correct, so do an assertion
	   */
	  if (!is_fields_file 
	      && LOOKUP(rec, INDEX_LBNREC) != 0
	      && LOOKUP(rec, INDEX_LBBEGIN) != 0
	      && data_offset_calculated != data_offset_specified)
	    {
	      error_mesg("read_all_headers_ff", "start of data record mismatch: %d %d",
			 data_offset_calculated, data_offset_specified);
	      ERR;
	    }
	  
	  data_offset_calculated += rec->disk_length * WORD_SIZE;
	  debug("did record %d %d", i_raw_rec, i_valid_rec);
	  i_valid_rec++;
	}
    }
      
  CKI(  free_(valid, heaplist)  );
  return 0;
  
  ERRBLKI("read_all_headers_ff");
}


int WITH_LEN(get_ff_disk_length)(INTEGER *ihdr)
{
  
  /* work out disk length */
  /* Input array size (packed field):
   *   First try LBNREC
   *   then if Cray 32-bit packing, know ratio of packed to unpacked lengths;
   *   else use LBLREC
   */
  size_t datalen;

  if (ihdr[INDEX_LBPACK] != 0 && ihdr[INDEX_LBNREC] != 0) 
    return ihdr[INDEX_LBNREC];
  if (ihdr[INDEX_LBPACK] % 10 ==2)
    {
      datalen = WITH_LEN(get_data_length)(ihdr);
      return datalen * 4 / WORD_SIZE;
    }
  return ihdr[INDEX_LBLREC];
}


/* 
 *  check which PP records are valid; populate an array provided by the caller with 1s and 0s
 *  and also provide the total count
 */
int WITH_LEN(get_valid_records_ff)(int fd,
				   Byte_ordering byte_ordering,
				   size_t hdr_start, size_t hdr_size, int n_raw_rec,
				   int valid[], int *n_valid_rec_return)
{
  int n_valid_rec, irec;
  INTEGER lbbegin;
  n_valid_rec = 0;
  
  for (irec = 0; irec < n_raw_rec; irec++)
    {
      valid[irec] = 0;
      CKI(   lseek(fd, hdr_start + irec * hdr_size + INDEX_LBBEGIN * WORD_SIZE, SEEK_SET)   );
      READ_ITEM(lbbegin);
      if (lbbegin != -99)
	{
	  /* valid record */
	  valid[irec] = 1;
	  n_valid_rec++;
	} 
      else
	valid[irec] = 0;
    }
  *n_valid_rec_return = n_valid_rec;
  return 0;

  ERRBLKI("get_valid_records_ff");
}

