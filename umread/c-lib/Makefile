TYPE_DEP_HDRS=bits/protos_sgl.h bits/protos_dbl.h
HEADERS=umfile.h umfileint.h \
	bits/datatype.h  bits/err_macros.h  bits/ordering.h  bits/pp_header.h  \
	bits/type_dep_protos.h  bits/type_indep_protos.h  bits/typedefs.h \
	$(TYPE_DEP_HDRS)


LIBRARY=umfile_test.so

CC=gcc
CFLAGS=-Wall -fPIC
CPP=gcc -E -P
LD=ld
LDFLAGS=-shared
TYPE_INDEP_OBJS= \
	umfile.o error.o filetype.o umfile_test.o \
	malloc.o linklist.o new_structs.o
TYPE_DEP_OBJ_STEMS=umfile_test_typedep interpret_header read process_vars \
	           debug_dump date_and_time

SGL_OBJS=$(foreach stem, $(TYPE_DEP_OBJ_STEMS), $(stem)_sgl.o)
DBL_OBJS=$(foreach stem, $(TYPE_DEP_OBJ_STEMS), $(stem)_dbl.o)
OBJS=$(TYPE_INDEP_OBJS) $(SGL_OBJS) $(DBL_OBJS)

.PHONY: clean all

all: $(LIBRARY)

clean:
	rm -f $(OBJS) $(TYPE_DEP_HDRS)

$(LIBRARY): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

bits/protos_sgl.h: bits/type_dep_protos.h
	$(CPP) -DBUILD_HDR -DSINGLE $< > $@

bits/protos_dbl.h: bits/type_dep_protos.h
	$(CPP) -DBUILD_HDR -DDOUBLE $< > $@

%_dbl.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -DDOUBLE -o $@ $<

%_sgl.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -DSINGLE -o $@ $<
