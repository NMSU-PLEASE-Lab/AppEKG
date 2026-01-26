
APPEKGDIR = ..

# on Mac, -Xclang may be needed
CC = gcc
CFLAGS = -I${APPEKGDIR} -Wall -fopenmp -g
LDFLAGS = -L${APPEKGDIR} -lappekg -lpthread
#  -lsqlite3 needed if AppEKG has it compiled in

all: hello binary binlinear binlinthread evensum dynamichb threadtest ubench

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

ubench: ubench.c
	$(CC) $(CFLAGS) -DAPPEKG_DISABLE -o $@-clean $< 
	$(CC) $(CFLAGS) -o $@ $< ${LDFLAGS}

utest:
	time -f "clean,%e,%U,%S,%e" ./ubench-clean 
	time -f "appekg,%e,%U,%S,%e" ./ubench
	time -f "clean,%e,%U,%S,%e" ./ubench-clean 
	time -f "appekg,%e,%U,%S,%e" ./ubench
	time -f "clean,%e,%U,%S,%e" ./ubench-clean 
	time -f "appekg,%e,%U,%S,%e" ./ubench
	time -f "clean,%e,%U,%S,%e" ./ubench-clean 
	time -f "appekg,%e,%U,%S,%e" ./ubench
	time -f "clean,%e,%U,%S,%e" ./ubench-clean 
	time -f "appekg,%e,%U,%S,%e" ./ubench


clean:
	rm -f *.o hello binary binlinear binlinthread evensum dynamichb threadtest ubench

