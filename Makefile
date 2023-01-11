PREFIX := ${HOME}

CC := gcc
STRIP := strip

CFLAGS := -O0 -std=c11 -Wall -D_POSIX_C_SOURCE -g
LDFLAGS :=

STRIPFLAGS := --strip-all -R .comment -x

all:
	ib ib.c.ib -i --flags "$(CFLAGS) $(LDFLAGS)"

install:
	cp ib ${PREFIX}/bin/

test: all
	./ib ib.c.ib -i --flags "$(CFLAGS) $(LDFLAGS)"

sa:
	cppcheck cppcheck --enable=all --inconclusive --library=posix --force --suppress=missingIncludeSystem ib.c
	frama-c ib.c -c11

clean:
	rm ib

bootstrap:
	$(CC) ib.c -o ib $(CFLAGS) $(LDFLAGS)
	cp ib ${PREFIX}/bin/
