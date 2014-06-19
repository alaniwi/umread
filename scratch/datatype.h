#if defined(DOUBLE)

#define REAL double
#define FUNC(x) x ## _dbl
#define COMPILED_TYPE (double_precision)

#elif defined(SINGLE)

#define REAL float
#define FUNC(x) x ## _sgl
#define COMPILED_TYPE  (single_precision)

#else
#error Need to compile this file with -DSINGLE or -DDOUBLE
#endif
