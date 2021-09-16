.POSIX:

CONFIGFILE = config.mk
include $(CONFIGFILE)


all: test
test.o: binary-multisearch.h

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.o:
	$(CC) -o $@ $< $(LDFLAGS)

check: test
	./test

install:
	mkdir -p -- "$(DESTDIR)$(PREFIX)/include"
	cp -- binary-multisearch.h "$(DESTDIR)$(PREFIX)/include"

uninstall:
	-rm -f -- "$(DESTDIR)$(PREFIX)/include/binary-multisearch.h"

clean:
	-rm -f -- *.o *.su test

.SUFFIXES:
.SUFFIXES: .o .c

.PHONY: all check install uninstall clean
