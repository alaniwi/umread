#include <stdlib.h>

#include "umfileint.h"


File *file_parse_core(int fd,
		      File_type file_type)
{
  File *file;
  List *heaplist;

  CKP(  file = new_file()  );
  file->fd = fd;
  file->file_type = file_type;

  heaplist = file->internp->heaplist;

  CKI(  read_all_headers(file, heaplist)  );
  debug_dump_all_headers(file);
  CKI(  process_vars(file, heaplist)  );

  return file;

 err:
  if (file)
    free_file(file);
  return NULL;
}


int process_vars(File *file, List *heaplist)
{
  int nrec;
  Rec **recs;

  nrec = file->internp->nrec;
  recs = file->internp->recs;
  
  CKI(   initialise_records(recs, nrec, heaplist)   );

  /* sort the records */
  qsort(recs, nrec, sizeof(Rec*), compare_records);
  
  return 0;
  ERRBLKI;
}


int initialise_records(Rec **recs, int nrec, List *heaplist) 
{
  int recno;
  Rec *rec;

  for (recno = 0; recno < nrec ; recno++)
    {
      rec = recs[recno];
      rec->internp = rec->internp;

      rec->internp->disambig_index = -1;
      rec->internp->supervar_index = -1;

      /* store level info */
      CKP(  rec->internp->lev = malloc_(sizeof(Level), heaplist)  );
      CKI(  lev_set(rec->internp->lev, rec)  );

      /* store time info */
      CKP(  rec->internp->time = malloc_(sizeof(Time), heaplist)  );
      CKI(  time_set(rec->internp->time, rec)  );
      rec->internp->mean_period = mean_period(rec->internp->time);
  }
  //  return 0;
  ERRBLKI;
}


