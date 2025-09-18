
APPEKGDIR = ..

# on Mac, -Xclang may be needed
CC = gcc
CFLAGS = -I${APPEKGDIR} -Wall -fopenmp
LDFLAGS = -L${APPEKGDIR} -lappekg -lpthread -lsqlite3

all: hello binary binlinear binlinthread evensum dynamichb threadtest

hello: HelloWorld.c
	$(CC) $(CFLAGS) -o $@ $< ${LDFLAGS}

binary: BinarySearch.c
	$(CC) $(CFLAGS) -o $@ $< ${LDFLAGS}

binlinear: BinaryVsLinear.c
	$(CC) $(CFLAGS) -o $@ $< ${LDFLAGS}

binlinthread: BinaryVsLinearMultithreading.c
	$(CC) $(CFLAGS) -o $@ $< ${LDFLAGS}

evensum: EvenNumbersSum.c
	$(CC) $(CFLAGS) -o $@ $< ${LDFLAGS}

dynamichb: DynamicHBGenerator.c
	$(CC) $(CFLAGS) -o $@ $< ${LDFLAGS}

threadtest: threadtest.c
	$(CC) $(CFLAGS) -o $@ $< ${LDFLAGS}

clean:
	rm -f *.o hello binary binlinear binlinthread evensum dynamichb threadtest
