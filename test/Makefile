
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
	$(CC) $(CFLAGS) -o $@-hb1 $< ${LDFLAGS}
	$(CC) $(CFLAGS) -DHBRATE=10 -o $@-hb10 $< ${LDFLAGS}
	$(CC) $(CFLAGS) -DHBRATE=100 -o $@-hb100 $< ${LDFLAGS}
	$(CC) $(CFLAGS) -DHBRATE=1000 -o $@-hb1000 $< ${LDFLAGS}

utest:
	time -f "clean,%U,%S,%e" ./ubench-clean 
	time -f "appekg1,%U,%S,%e" ./ubench-hb1
	time -f "appekg10,%U,%S,%e" ./ubench-hb10
	time -f "appekg100,%U,%S,%e" ./ubench-hb100
	time -f "appekg1000,%U,%S,%e" ./ubench-hb1000


clean:
	rm -f *.o hello binary binlinear binlinthread evensum dynamichb threadtest ubench

