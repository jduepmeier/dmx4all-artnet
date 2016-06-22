export PREFIX?=/usr
export DOCDIR?=$(DESTDIR)$(PREFIX)/share/man/man1

.PHONY: all clean
CFLAGS?=-g -Wall
LDLIBS?=-lm

all: art-node art-node.1.gz

install:
	install -m 0755 art-node "$(DESTDIR)$(PREFIX)/bin"
	install -g 0 -o 0 -m 0644 art-node.1.gz "$(DOCDIR)"

art-node.1.gz:
	gzip -c < art-node.1 > $@

clean:
	$(RM) art-node art-node.1.gz

displaytest:
	valgrind -v --leak-check=full --track-origins=yes --show-reachable=yes ./art-node 16
