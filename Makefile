#
# Build file for AppEKG
#

# OpenMP support (ON/OFF)
DO_USE_OPENMP = ON
ifeq ($(DO_USE_OPENMP),ON)
OMPFLAGS = -DEKG_USE_OPENMP -fopenmp
else 
OMPFLAGS = 
endif

# LDMS streams support
DO_USE_LDMS_STREAMS = OFF
LDMSDIR = /project/hpcjobquality/tools/INSTALL/OVIS
ifeq ($(DO_USE_LDMS_STREAMS),ON)
LDMSFLAGS = -DUSE_LDMS_STREAMS -I${LDMSDIR}/include
else 
LDMSFLAGS = 
endif

# -DDEBUG is allowable
CFLAGS = ${OMPFLAGS} ${LDMSFLAGS}

.phony: all doc clean

all: libappekg.a 

libappekg.a: appekg.o
	ar crs libappekg.a appekg.o

appekg.o: appekg.c appekg.h
	gcc -Wall -c $(CFLAGS) appekg.c

clean:
	rm -rf *.o libappekg.a

doc: Doxyfile appekg.c appekg.h
	doxygen Doxyfile

