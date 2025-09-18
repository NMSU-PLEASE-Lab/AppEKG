#
# Build file for AppEKG
#

# options
DO_USE_OPENMP = OFF
DO_USE_SQLITE3 = ON
DO_USE_LDMS_STREAMS = OFF

# OpenMP support (ON/OFF)
ifeq ($(DO_USE_OPENMP),ON)
OMPCFLAGS = -DEKG_USE_OPENMP -fopenmp
OMPLFLAGS = 
else 
OMPCFLAGS = 
OMPLFLAGS = 
endif

# LDMS streams support
LDMSDIR = /project/hpcjobquality/tools/INSTALL/OVIS
ifeq ($(DO_USE_LDMS_STREAMS),ON)
LDMSCFLAGS = -DINCLUDE_LDMS -I${LDMSDIR}/include
LDMSLFLAGS = 
else 
LDMSFLAGS = 
LDMSFLAGS = 
endif

# SQLite3 streams support
SQLITE_DIR = /usr/local/sqlite3
ifeq ($(DO_USE_SQLITE3),ON)
SQLCFLAGS = -DINCLUDE_SQLITE -DSQLITE_DBNAME=\"appekg-test.db3\" -I${SQLITE_DIR}/include
else 
SQLCFLAGS = 
endif

CC = gcc
AR = ar
# -DDEBUG is allowable
CFLAGS = -Wall ${OMPCFLAGS} ${LDMSCFLAGS} ${SQLCFLAGS}
LDFLAGS = ${OMPLFLAGS} ${LDMSLFLAGS} ${SQLLFLAGS}

.phony: all doc clean

all: libappekg.a 

libappekg.a: appekg.o
	$(AR) crs libappekg.a appekg.o

appekg.o: appekg.c appekg.h csvoutput.c ldmsoutput.c sqliteoutput.c
	$(CC) $(CFLAGS) -c appekg.c

clean:
	rm -rf *.o libappekg.a

doc: Doxyfile appekg.c appekg.h
	doxygen Doxyfile

