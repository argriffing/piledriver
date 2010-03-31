FLAGS = -Wall -Wextra -std=c99 -pedantic -pipe

.PHONY: all
all : piledriver pilesplitter acgtn-to-rabc summarize-acgtn summarize-pileup extract-pileup-chromosome pileup-to-acgtn

piledriver : piledriver.o xgutil.o pileutil.o
	gcc piledriver.o xgutil.o pileutil.o -o piledriver
piledriver.o : piledriver.c
	gcc ${FLAGS} -c piledriver.c

pileup-to-acgtn : pileup-to-acgtn.o xgutil.o
	gcc pileup-to-acgtn.o xgutil.o -o pileup-to-acgtn
pileup-to-acgtn.o : pileup-to-acgtn.c
	gcc ${FLAGS} -c pileup-to-acgtn.c

pilesplitter : pilesplitter.o xgutil.o
	gcc pilesplitter.o xgutil.o -o pilesplitter
pilesplitter.o : pilesplitter.c
	gcc ${FLAGS} -c pilesplitter.c

extract-pileup-chromosome : extract-pileup-chromosome.o xgutil.o
	gcc extract-pileup-chromosome.o xgutil.o -o extract-pileup-chromosome
extract-pileup-chromosome.o : extract-pileup-chromosome.c
	gcc ${FLAGS} -c extract-pileup-chromosome.c

acgtn-to-rabc : acgtn-to-rabc.o
	gcc acgtn-to-rabc.o -o acgtn-to-rabc
acgtn-to-rabc.o : acgtn-to-rabc.c
	gcc ${FLAGS} -c acgtn-to-rabc.c

summarize-acgtn : summarize-acgtn.o xgutil.o
	gcc summarize-acgtn.o xgutil.o -o summarize-acgtn
summarize-acgtn.o : summarize-acgtn.c
	gcc ${FLAGS} -c summarize-acgtn.c

summarize-pileup : summarize-pileup.o xgutil.o summaryutil.o pileutil.o
	gcc summarize-pileup.o xgutil.o summaryutil.o pileutil.o -o summarize-pileup

summarize-pileup.o : summarize-pileup.c
	gcc ${FLAGS} -c summarize-pileup.c


pileutil.o : pileutil.c pileutil.h
	gcc ${FLAGS} -c pileutil.c

xgutil.o : xgutil.c xgutil.h
	gcc ${FLAGS} -c xgutil.c

summaryutil.o : summaryutil.c summaryutil.h
	gcc ${FLAGS} -c summaryutil.c


clean :
	rm *.o
