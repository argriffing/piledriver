.PHONY: all
all : piledriver pilesplitter

piledriver : piledriver.o
	gcc piledriver.o -o piledriver

pilesplitter : pilesplitter.o
	gcc pilesplitter.o -o pilesplitter

piledriver.o : piledriver.c
	gcc -c piledriver.c

pilesplitter.o : pilesplitter.c
	gcc -c pilesplitter.c

clean :
	rm piledriver.o
	rm pilesplitter.o
