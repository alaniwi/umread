#include <string.h>

#include "umfileint.h"

T_axis *new_t_axis(List *heaplist)
{
  T_axis *t_axis;
  CKP(  t_axis = malloc_(sizeof(T_axis), heaplist)  );
  t_axis->values = list_new(heaplist);
  t_axis->type = -1;   //FIXME: used?
  // t_axis->dimid = -1;   //FIXME: used?
  memset(&t_axis->time_orig, 0, sizeof(Date));    //FIXME: used?
  return t_axis;
  ERRBLKP;
}

int free_t_axis(T_axis *t_axis, List *heaplist)
{
  CKI(  list_free(t_axis->values, 1, heaplist)  );
  CKI(  free_(t_axis, heaplist)   );
  return 0;
  ERRBLKI;
}


Z_axis *new_z_axis(List *heaplist)
{
  Z_axis *z_axis;
  CKP(  z_axis = malloc_(sizeof(Z_axis), heaplist)  );
  z_axis->values = list_new(heaplist);
  // z_axis->dimid = -1; //FIXME: used?
  z_axis->lev_type = -1; //FIXME: used?
  return z_axis;
  ERRBLKP;
}

int free_z_axis(Z_axis *z_axis, List *heaplist)
{
  CKI(  list_free(z_axis->values, 1, heaplist)  );
  CKI(  free_(z_axis, heaplist)  );
  return 0;
  ERRBLKI;
}


int t_axis_add(T_axis *t_axis, const Time *time, 
	       int *index_return, List *heaplist)
{
  Time *timecopy;

  CKP(   timecopy = dup_(time, sizeof(Time), heaplist)   );
  return list_add_or_find(t_axis->values, &timecopy, compare_times, 0, 
			  free_, index_return, heaplist);
  ERRBLKI;
}



int z_axis_add(Z_axis *z_axis, const Level *lev, 
	      int *index_return, List *heaplist)
{
  Level *levcopy;

  CKP(   levcopy = dup_(lev, sizeof(Level), heaplist)   );
  return list_add_or_find(z_axis->values, &levcopy, compare_levels, 0, 
			  free_, index_return, heaplist);
  ERRBLKI;
}

