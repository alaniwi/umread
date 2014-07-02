TYPE_DEP_HDRS=protos_sgl.h protos_dbl.h
HEADERS=umfile.h umfileint.h datatype.h $(TYPE_DEP_HDRS)
LIBRARY=umfile_test.so

CC=gcc
CFLAGS=-Wall -fPIC
CPP=gcc -E -P
LD=ld
LDFLAGS=-shared
TYPE_INDEP_OBJS=umfile_test.o
TYPE_DEP_OBJ_STEMS=umfile_test_typedep

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

protos_sgl.h: type_dep_protos.h
	$(CPP) -DBUILD_HDR -DSINGLE $< > $@

protos_dbl.h: type_dep_protos.h
	$(CPP) -DBUILD_HDR -DDOUBLE $< > $@

%_dbl.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -DDOUBLE -o $@ $<

%_sgl.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -DSINGLE -o $@ $<
