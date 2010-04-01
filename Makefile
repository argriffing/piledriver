CFLAGS = -Wall -Wextra -std=c99 -pedantic -pipe
CPPFLAGS = -Wall -Wextra -pedantic -pipe

MYCC = gcc
MYCPP = g++

.PHONY: all
all : piledriver pilesplitter acgtn-to-rabc summarize-acgtn \
	summarize-pileup extract-pileup-chromosome pileup-to-acgtn \
	get-pileup-chromosome-names

piledriver : piledriver.o xgutil.o pileutil.o
	${MYCC} piledriver.o xgutil.o pileutil.o -o piledriver
piledriver.o : piledriver.c
	${MYCC} ${CFLAGS} -c piledriver.c

pileup-to-acgtn : pileup-to-acgtn.o xgutil.o
	${MYCC} pileup-to-acgtn.o xgutil.o -o pileup-to-acgtn
pileup-to-acgtn.o : pileup-to-acgtn.c
	${MYCC} ${CFLAGS} -c pileup-to-acgtn.c

pilesplitter : pilesplitter.o xgutil.o
	${MYCC} pilesplitter.o xgutil.o -o pilesplitter
pilesplitter.o : pilesplitter.c
	${MYCC} ${CFLAGS} -c pilesplitter.c

extract-pileup-chromosome : extract-pileup-chromosome.o xgutil.o
	${MYCC} extract-pileup-chromosome.o xgutil.o \
		-o extract-pileup-chromosome
extract-pileup-chromosome.o : extract-pileup-chromosome.c
	${MYCC} ${CFLAGS} -c extract-pileup-chromosome.c

acgtn-to-rabc : acgtn-to-rabc.o
	${MYCC} acgtn-to-rabc.o -o acgtn-to-rabc
acgtn-to-rabc.o : acgtn-to-rabc.c
	${MYCC} ${CFLAGS} -c acgtn-to-rabc.c

summarize-acgtn : summarize-acgtn.o xgutil.o
	${MYCC} summarize-acgtn.o xgutil.o -o summarize-acgtn
summarize-acgtn.o : summarize-acgtn.c
	${MYCC} ${CFLAGS} -c summarize-acgtn.c

summarize-pileup : summarize-pileup.o xgutil.o summaryutil.o pileutil.o
	${MYCC} summarize-pileup.o xgutil.o summaryutil.o pileutil.o \
		-o summarize-pileup
summarize-pileup.o : summarize-pileup.c
	${MYCC} ${CFLAGS} -c summarize-pileup.c

get-pileup-chromosome-names : get-pileup-chromosome-names.o xgutil.o
	${MYCPP} get-pileup-chromosome-names.o xgutil.o \
		-o get-pileup-chromosome-names
get-pileup-chromosome-names.o : get-pileup-chromosome-names.C
	${MYCPP} ${CPPFLAGS} -c get-pileup-chromosome-names.C


pileutil.o : pileutil.c pileutil.h
	${MYCC} ${CFLAGS} -c pileutil.c

xgutil.o : xgutil.c xgutil.h
	${MYCC} ${CFLAGS} -c xgutil.c

summaryutil.o : summaryutil.c summaryutil.h
	${MYCC} ${CFLAGS} -c summaryutil.c


clean :
	rm *.o
