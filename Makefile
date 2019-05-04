SHELL=/bin/sh

default: crush

install: crush
	@(test $$(id -u) -ne 0 && \
	  echo 'You need to ask root to install.' && \
	  exit 0) || cp crush /usr/local/bin

crush: crush.o
	gcc -s -o crush crush.o -ltestu01 -lprobdist -lmylib -lm

crush.o: crush.c
	gcc -O2 -c crush.c

all: crush doc
	notangle -Rcrush.c crush.nw > crush.c

doc: crush.nw
	noweave -filter 'dpp -tex' -delay -index -autodefs c crush.nw > crush.tex
	pdflatex crush.tex

clean: 
	rm -f *.dvi *.log *.aux *.bbl *.bcf *.blg *.run.xml
	rm -f crush *.o
