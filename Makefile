CC              = g++ -std=c++11
CC_OBJ_FLAGS    = -c -fPIC
CC_Shared_FLAGS = -shared -Wl,-soname,libTrigDiag.so
ROOT_CFLAGS     = $(shell /usr/local/apps/root_build/bin/root-config --cflags)
ROOT_LIBS       = $(shell /usr/local/apps/root_build/bin/root-config --libs)
CODA		= /Work/apps/coda
CODA_FLAGS	= -I${CODA}/Linux-x86_64/include
HLS_FLAGS 	= -I/Work/apps/Vivado_HLS/2015.4/include
libHPSTrg	= libHPSTrg


all:		HPSTrg.o
		rm -f ${libHPSTrg}.so*
		$(CC) $(CC_Shared_FLAGS) -o lib/${libHPSTrg}.so.1.0.1 $?
		cd lib;\
		ln -sf ${libHPSTrg}.so.1.0.1 ${libHPSTrg}.so.1; ln -sf ${libHPSTrg}.so.1.0.1 ${libHPSTrg}.so
	
HPSTrg.o:	src/THPSTrig.cc include/THPSTrig.h
		$(CC) $(CC_OBJ_FLAGS) src/THPSTrig.cc -o $@ $(ROOT_CFLAGS) $(CODA_FLAGS) $(HLS_FLAGS) -I./include
	
	
clean:
		rm -f lib/*.so.* lib/*.so *.o
