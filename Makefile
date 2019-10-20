SHELL=/bin/sh

default: crush

compile: \
warn-auto.sh conf-cc
	( cat warn-auto.sh; \
	echo exec "`head -1 conf-cc`" '-c $${1+"$$@"}' \
	) > compile
	chmod 755 compile

choose: \
warn-auto.sh choose.sh conf-home
	cat warn-auto.sh choose.sh > choose
	chmod 755 choose

load: \
warn-auto.sh conf-ld
	( cat warn-auto.sh; \
	echo 'main="$$1"; shift'; \
	echo exec "`head -1 conf-ld`" \
	'-o "$$main" "$$main".o $${1+"$$@"}' \
	) > load
	chmod 755 load

makelib: \
warn-auto.sh systype
	( cat warn-auto.sh; \
	echo 'main="$$1"; shift'; \
	echo 'rm -f "$$main"'; \
	echo 'ar cr "$$main" $${1+"$$@"}'; \
	case "`cat systype`" in \
	sunos-5.*) ;; \
	unix_sv*) ;; \
	irix64-*) ;; \
	irix-*) ;; \
	dgux-*) ;; \
	hp-ux-*) ;; \
	sco*) ;; \
	*) echo 'ranlib "$$main"' ;; \
	esac \
	) > makelib
	chmod 755 makelib

systype: \
find-systype.sh conf-cc conf-ld trycpp.c
	( cat warn-auto.sh; \
	echo CC=\'`head -1 conf-cc`\'; \
	echo LD=\'`head -1 conf-ld`\'; \
	cat find-systype.sh; \
	) | sh > systype

install: crush
	@(test $$(id -u) -ne 0 && \
	  echo 'You need to ask root to install.' && \
	  exit 0) || cp crush /usr/local/bin

uninstall:
	@(test $$(id -u) -ne 0 && \
	  echo 'You need to ask root to uninstall.' && \
	  exit 0) || rm -f /usr/local/bin/crush

clean: 
	rm -f *.dvi *.log *.aux *.bbl *.bcf *.blg *.run.xml crush.pdf crush.tex
	rm -f crush *.o

crush: crush.o
	gcc -s -o crush crush.o -ltestu01 -lprobdist -lmylib -lm

crush.o: crush.c
	gcc -O2 -c crush.c

weave: crush.nw
	any weave -filter 'dpp -tex' -delay -index crush.nw > crush.tex

tangle: crush.nw
	any tangle -Rcrush.c crush.nw > crush.c

crush.tex: weave

pdf: crush.tex
	pdflatex crush && biber crush && pdflatex crush && pdflatex crush

package:
	c:/emacs/git/bin/git archive --format=tar --prefix=crush-$(v)/ HEAD | gzip > crush-$(v).tar.gz
