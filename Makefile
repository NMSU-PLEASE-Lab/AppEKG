#
# Build file for AppEKG
#
# TODO: the linking flags (...LFLAGS) are not used to build the
#       AppEKG library, since it is packaged as a static library.
#       But some options, like SQLite, will need linking flags
#       when the instrumented application is built. The ones here
#       can serve as examples. Perhaps we should build a DLL?
#

# Options
DO_USE_OPENMP = ON
DO_USE_SQLITE3 = OFF
DO_USE_LDMS_STREAMS = OFF

# OpenMP support (ON/OFF); this is for how thread IDs are obtained
# TODO: make this dynamic? OpenMP _only_ works for OpenMP programs,
# and pthread IDs only work for pthreads. I wish both could work.
ifeq ($(DO_USE_OPENMP),ON)
OMPCFLAGS = -DEKG_USE_OPENMP -fopenmp
OMPLFLAGS = -lopenmp
else 
OMPCFLAGS = 
OMPLFLAGS = 
endif

# LDMS streams data output support
LDMSDIR = /project/hpcjobquality/tools/INSTALL/OVIS
ifeq ($(DO_USE_LDMS_STREAMS),ON)
LDMSCFLAGS = -DINCLUDE_LDMS -I${LDMSDIR}/include
LDMSLFLAGS = -L${LDMSDIR}/lib -lldms
else 
LDMSCFLAGS = 
LDMSLFLAGS = 
endif

# SQLite3 data output support
SQLITE_DIR = /usr/local/sqlite3
ifeq ($(DO_USE_SQLITE3),ON)
SQLCFLAGS = -DINCLUDE_SQLITE -DSQLITE_DBNAME=\"appekg-test.db3\" -I${SQLITE_DIR}/include
SQLLFLAGS = -L${SQLITE_DIR}/lib -lsqlite3
else 
SQLCFLAGS = 
SQLLFLAGS = 
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

doc: Doxyfile appekg.c appekg.h csvoutput.c ldmsoutput.c sqliteoutput.c
	doxygen Doxyfile

