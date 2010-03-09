.PHONY: all
all : piledriver pilesplitter elementwise-sum acgtn-to-rabc

piledriver : piledriver.o
	gcc piledriver.o -o piledriver
piledriver.o : piledriver.c
	gcc -c piledriver.c

pilesplitter : pilesplitter.o
	gcc pilesplitter.o -o pilesplitter
pilesplitter.o : pilesplitter.c
	gcc -c pilesplitter.c

elementwise-sum : elementwise-sum.o
	gcc elementwise-sum.o -o elementwise-sum
elementwise-sum.o : elementwise-sum.c
	gcc -c elementwise-sum.c

acgtn-to-rabc : acgtn-to-rabc.o
	gcc acgtn-to-rabc.o -o acgtn-to-rabc
acgtn-to-rabc.o : acgtn-to-rabc.c
	gcc -c acgtn-to-rabc.c

clean :
	rm piledriver.o
	rm pilesplitter.o
	rm elementwise-sum.o
	rm acgtn-to-rabc.o
