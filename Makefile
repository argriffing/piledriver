piledriver : piledriver.o
	gcc -lm piledriver.o -o piledriver

piledriver.o : piledriver.c
	gcc -c piledriver.c

clean :
	rm piledriver.o
