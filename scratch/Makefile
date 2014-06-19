TYPE_DEP_HDRS=protos_sgl.h protos_dbl.h
HEADERS=mypackage.h datatype.h $(TYPE_DEP_HDRS)
EXE=mytest

CC=gcc
CFLAGS=-Wall
CPP=gcc -E -P
LD=gcc
TYPE_INDEP_OBJS=dispatch.o main.o
TYPE_DEP_OBJ_STEMS=multiply

SGL_OBJS=$(foreach stem, $(TYPE_DEP_OBJ_STEMS), $(stem)_sgl.o)
DBL_OBJS=$(foreach stem, $(TYPE_DEP_OBJ_STEMS), $(stem)_dbl.o)
OBJS=$(TYPE_INDEP_OBJS) $(SGL_OBJS) $(DBL_OBJS)

.PHONY: clean all

all: $(EXE)

clean:
	rm -f $(OBJS) $(TYPE_DEP_HDRS)

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) -o $(EXE) $(OBJS)

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
