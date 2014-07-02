#if defined(DOUBLE)

#define REAL real8
#define INTEGER int8
#define WITH_TYPE(x) x ## _dbl
#define COMPILED_TYPE (double_precision)

#elif defined(SINGLE)

#define REAL real4
#define INTEGER int4
#define WITH_TYPE(x) x ## _sgl
#define COMPILED_TYPE  (single_precision)

#else
#error Need to compile this file with -DSINGLE or -DDOUBLE
#endif
