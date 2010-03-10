.PHONY: all
all : piledriver pilesplitter elementwise-sum acgtn-to-rabc

piledriver : piledriver.o xgutil.o
	gcc piledriver.o xgutil.o -o piledriver
piledriver.o : piledriver.c
	gcc -c piledriver.c

pilesplitter : pilesplitter.o xgutil.o
	gcc pilesplitter.o xgutil.o -o pilesplitter
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

xgutil : xgutil.o
	gcc xgutil.o -o xgutil
xgutil.o : xgutil.c
	gcc -c xgutil.c

clean :
	rm piledriver.o
	rm pilesplitter.o
	rm elementwise-sum.o
	rm acgtn-to-rabc.o
	rm xgutil.o
