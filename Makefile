CC=gcc
CFLAGS=-no-pie
DEPS = HP.h HT.h Record.h BF.h
EXECS = mainHP mainHT

all:
	$(CC) -o mainHP mainHP.c HP.c BF_64.a $(CFLAGS)
	$(CC) -o mainHT mainHT.c HT.c BF_64.a $(CFLAGS)

HP:
	$(CC) -o mainHP mainHP.c HP.c BF_64.a $(CFLAGS)

HT:
	$(CC) -o mainHT mainHT.c HT.c BF_64.a $(CFLAGS)

clear:
	rm $(EXECS)
	rm *.swp
