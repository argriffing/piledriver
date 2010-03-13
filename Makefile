.PHONY: all
all : piledriver pilesplitter elementwise-sum acgtn-to-rabc summarize-acgtn summarize-pileup

piledriver : piledriver.o xgutil.o pileutil.o
	gcc piledriver.o xgutil.o pileutil.o -o piledriver
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

summarize-acgtn : summarize-acgtn.o xgutil.o
	gcc summarize-acgtn.o xgutil.o -o summarize-acgtn
summarize-acgtn.o : summarize-acgtn.c
	gcc -c summarize-acgtn.c

summarize-pileup : summarize-pileup.o xgutil.o summaryutil.o pileutil.o
	gcc summarize-pileup.o xgutil.o summaryutil.o pileutil.o -o summarize-pileup

summarize-pileup.o : summarize-pileup.c
	gcc -c summarize-pileup.c

pileutil.o : pileutil.c
	gcc -c pileutil.c

xgutil.o : xgutil.c
	gcc -c xgutil.c

summaryutil.o : summaryutil.c
	gcc -c summaryutil.c


clean :
	rm *.o
