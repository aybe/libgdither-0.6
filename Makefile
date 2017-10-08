VERSION = 0.6
PREFIX = /usr/local
CFLAGS = -Wall -g -I. -O3 -funroll-loops -finline-limit=9999999 -ffast-math -malign-double -march=`uname -m` -fPIC

TARDIR = libgdither-$(VERSION)

all: libgdither.so

gdither.o: gdither.c gdither.h gdither_types_internal.h
	$(CC) $(CFLAGS) -c gdither.c

libgdither.so: gdither.o
	$(CC) -shared gdither.o -o libgdither.so

libgdither.pc: libgdither.pc.in Makefile
	sed -e 's|@VERSION@|$(VERSION)|' -e 's|@PREFIX@|$(PREFIX)|' \
		libgdither.pc.in > libgdither.pc

install: all
	install -d $(PREFIX)/lib/
	install libgdither.so $(PREFIX)/lib/
	install -d $(PREFIX)/include/
	install gdither.h $(PREFIX)/include/
	install gdither_types.h $(PREFIX)/include/
	install -d $(PREFIX)/lib/pkgconfig/
	install libgdither.pc $(PREFIX)/lib/pkgconfig/

clean:
	rm -f *.o *.so
	(cd tests && make clean)
	(cd examples && make clean)

dist: all libgdither.pc
	rm -rf $(TARDIR)
	mkdir -p $(TARDIR)/tests $(TARDIR)/examples
	cp README COPYING Makefile *.c *.h libgdither.pc \
		$(TARDIR)
	cp tests/Makefile tests/*.[ch] tests/*.cc \
		$(TARDIR)/tests/ 
	cp examples/Makefile examples/*.[ch] \
		$(TARDIR)/examples/ 
	(cd $(TARDIR) && make test clean)
	tar cfz $(TARDIR).tar.gz $(TARDIR)
	@rm -rf $(TARDIR)

test: all
	(cd tests && make test)
