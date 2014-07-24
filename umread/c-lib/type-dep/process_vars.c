#include <stdlib.h>

#include "umfileint.h"

int WITH_LEN(process_vars)(File *file, List *heaplist)
{
  int nrec;
  Rec **recs;

  nrec = file->internp->nrec;
  recs = file->internp->recs;
  
  CKI(   WITH_LEN(initialise_records)(recs, nrec, heaplist)   );

  /* sort the records */
  //qsort(recs, nrec, sizeof(Rec*), compare_records);
  
  // ....

  ERRBLKI("process_vars");
}


int WITH_LEN(initialise_records)(Rec **recs, int nrec, List *heaplist) 
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
      CKI(  WITH_LEN(lev_set)(rec->internp->lev, rec)  );

      /* store time info */
      CKP(  rec->internp->time = malloc_(sizeof(Time), heaplist)  );
      CKI(  WITH_LEN(time_set)(rec->internp->time, rec)  );
      rec->internp->mean_period = WITH_LEN(mean_period)(rec->internp->time);
  }
  return 0;

  ERRBLKI("initialise_records");
}


int WITH_LEN(lev_set)(Level *lev, Rec *rec) {
  
  lev->type         = WITH_LEN(level_type)(rec);

  switch (lev->type) 
    {
    case hybrid_height_lev_type:
      lev->values.hybrid_height.a = RLOOKUP(rec, INDEX_BLEV);
      lev->values.hybrid_height.b = RLOOKUP(rec, INDEX_BHLEV);
#ifdef BDY_LEVS
      lev->values.hybrid_height.ubdy_a = RLOOKUP(rec, INDEX_BULEV);
      lev->values.hybrid_height.ubdy_b = RLOOKUP(rec, INDEX_BHULEV);
      lev->values.hybrid_height.lbdy_a = RLOOKUP(rec, INDEX_BRLEV);
      lev->values.hybrid_height.lbdy_b = RLOOKUP(rec, INDEX_BHRLEV);
#endif
      break;

    case hybrid_sigmap_lev_type:
      lev->values.hybrid_sigmap.a = RLOOKUP(rec, INDEX_BHLEV);
      lev->values.hybrid_sigmap.b = RLOOKUP(rec, INDEX_BLEV);
#ifdef BDY_LEVS
      lev->values.hybrid_sigmap.ubdy_a = RLOOKUP(rec, INDEX_BHULEV);
      lev->values.hybrid_sigmap.ubdy_b = RLOOKUP(rec, INDEX_BULEV);
      lev->values.hybrid_sigmap.lbdy_a = RLOOKUP(rec, INDEX_BHRLEV);
      lev->values.hybrid_sigmap.lbdy_b = RLOOKUP(rec, INDEX_BRLEV);
#endif
      break;

    case pseudo_lev_type:
      lev->values.pseudo.index = LOOKUP(rec, INDEX_LBUSER5);
      break;
      
    default:
      if (RLOOKUP(rec, INDEX_BLEV) == 0 
	  && LOOKUP(rec, INDEX_LBLEV) != 9999
	  && LOOKUP(rec, INDEX_LBLEV) != 8888) 
	lev->values.misc.level = LOOKUP(rec, INDEX_LBLEV);
      else
	lev->values.misc.level = RLOOKUP(rec, INDEX_BLEV);
#ifdef BDY_LEVS
      lev->values.misc.ubdy_level = RLOOKUP(rec, INDEX_BULEV);
      lev->values.misc.lbdy_level = RLOOKUP(rec, INDEX_BRLEV);
#endif
      break;
    }
  return 0;
}


Lev_type WITH_LEN(level_type)(const Rec *rec) 
{
  if (LOOKUP(rec, INDEX_LBUSER5) != 0 
      && LOOKUP(rec, INDEX_LBUSER5) != int_missing_data)
    return pseudo_lev_type;
  
  switch (LOOKUP(rec, INDEX_LBVC))
    {
      /*
       *         1  Height (m)              8  Pressure (mb)
       *         9  Hybrid co-ordinates    10  Sigma (=p/p*)
       *         128  Mean sea level      129  Surface
       *         130  Tropopause level    131  Maximum wind level
       *         132  Freezing level      142  Upper hybrid level
       *         143  Lower hybrid level  176  Latitude (deg)
       *         177  Longitude (deg)
       */
      /* also new dynamics:  65  hybrid height */
      
    case 1:
      return height_lev_type;
    case 2:
      return depth_lev_type;
    case 5:
      return boundary_layer_top_lev_type;
    case 6:
      return soil_lev_type;
    case 8:
      return pressure_lev_type;
    case 9:
      return hybrid_sigmap_lev_type;
    case 65:
      return hybrid_height_lev_type;
    case 128:
      return mean_sea_lev_type;
    case 129:
      return surface_lev_type;
    case 130:
      return tropopause_lev_type;
    case 133:
      return top_of_atmos_lev_type;
    default:
      return other_lev_type;
    }
}

