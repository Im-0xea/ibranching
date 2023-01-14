PREFIX := /usr

CC := gcc
STRIP := strip

CFLAGS := -O0 -std=gnu17 -Wall -g
LDFLAGS := 

all:
	ib ib.c.ib -in --flags "$(CFLAGS) $(LDFLAGS)"

install:
	cp ib ${PREFIX}/bin/

test: all
	./ib ib.c.ib -in --flags "$(CFLAGS) $(LDFLAGS)"
	cppcheck cppcheck --enable=all --inconclusive --library=posix --force --suppress=missingIncludeSystem ib.c
	make -C testing

clean:
	rm -f *.c testing/*.c

bootstrap:
	$(CC) ib.c -o ib $(CFLAGS) $(LDFLAGS)
	cp ib ${PREFIX}/bin/
