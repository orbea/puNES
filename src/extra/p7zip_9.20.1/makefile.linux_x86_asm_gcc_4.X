#
# makefile for Linux (x86, PPC, alpha ...)
#

OPTFLAGS=-O

ALLFLAGS=${OPTFLAGS} -pipe -m32 -s \
    	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-DNDEBUG -D_REENTRANT -DENV_UNIX \
	-D_7ZIP_LARGE_PAGES \
	$(LOCAL_FLAGS)

CXX=g++ $(ALLFLAGS) -DENV_HAVE_GCCVISIBILITYPATCH -fvisibility=hidden -fvisibility-inlines-hidden
CC=gcc $(ALLFLAGS)
CC_SHARED=-fPIC
LINK_SHARED=-fPIC -shared
ASM=nasm -f elf

PRE_COMPILED_HEADER=StdAfx.h.gch

LOCAL_LIBS=-lpthread
LOCAL_LIBS_DLL=$(LOCAL_LIBS) -ldl

CPU=x86
OBJ_CRC32=$(OBJ_CRC32_x86)

