CC              = g++ -std=c++11
CC_OBJ_FLAGS    = -c -fPIC
CC_Shared_FLAGS = -shared -Wl,-soname,libHPSTrg.so
ROOT_CFLAGS     = $(shell /usr/local/apps/root_build/bin/root-config --cflags)
ROOT_LIBS       = $(shell /usr/local/apps/root_build/bin/root-config --libs)
CODA		= /Work/apps/coda
CODA_FLAGS	= -I${CODA}/Linux-x86_64/include
EVIO5p1		= /Work/apps/coda/evio-5.1/src
EVIO_FLAGS	= -I${EVIO5p1}/libsrc -I${EVIO5p1}/libsrc++
HLS_FLAGS 	= -I/Work/apps/Vivado_HLS/2015.4/include
libHPSTRG	= libHPSTrg


all:		HPSTrg.o
		rm -f ${libHPSTRG}.so*
		$(CC) $(CC_Shared_FLAGS) -o lib/${libHPSTRG}.so.1.0.1 $?
		cd lib;\
		ln -sf ${libHPSTRG}.so.1.0.1 ${libHPSTRG}.so.1; ln -sf ${libHPSTRG}.so.1.0.1 ${libHPSTRG}.so
	
HPSTrg.o:	src/THPSTrig.cc include/THPSTrig.h
		$(CC) $(CC_OBJ_FLAGS) src/THPSTrig.cc -o $@ $(ROOT_CFLAGS) $(EVIO_FLAGS) $(HLS_FLAGS) -I./include	
	
clean:
		rm -f lib/*.so.* lib/*.so *.o