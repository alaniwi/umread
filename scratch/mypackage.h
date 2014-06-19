typedef enum {single_precision, double_precision} data_type;

void multiply_dispatch(const void *, const void *, void *, data_type);

/* prototypes for datatype dependent functions are included via the following 
 * files automatically generated from type_dep_protos.h */

#include "protos_sgl.h"
#include "protos_dbl.h"

