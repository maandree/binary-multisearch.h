.POSIX:

CONFIGFILE = config.mk
include $(CONFIGFILE)

all: test

test: test.c binary-multisearch.h
	$(CC) -o $@ test.c $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)

check: test
	./test

install:
	mkdir -p -- "$(DESTDIR)$(PREFIX)/include"
	cp -- binary-multisearch.h "$(DESTDIR)$(PREFIX)/include"

uninstall:
	-rm -f -- "$(DESTDIR)$(PREFIX)/include/binary-multisearch.h"

clean:
	-rm -f -- *.o test

.PHONY: all check install uninstall clean
