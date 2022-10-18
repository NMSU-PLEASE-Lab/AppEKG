
APPEKGDIR = ..

CFLAGS = -I${APPEKGDIR} -Wall -fopenmp
LDOBJS = -L${APPEKGDIR} -lappekg -lpthread 

all: hello testHB evensum

hello: HelloWorld.o
	gcc -o $@ $< ${LDOBJS} 

testHB: BinarySearch.o
	gcc -o $@ $< ${LDOBJS} 

evensum: EvenNumbersSum.o
	gcc -fopenmp -o $@ $< ${LDOBJS} 

threadtest: threadtest.c
	gcc -o $@ -fopenmp -Wall threadtest.c

clean:
	rm -f *.o testHB evensum hello
