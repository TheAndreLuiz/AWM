.POSIX:
CC = g++
PKG_CONFIG = `pkg-config --cflags --libs xcb xcb-icccm xcb-keysyms xcb-image gtkmm-4.0 glib-2.0`
ALL_WARNING = -Wall -Wextra -pedantic
ALL = $(PKG_CONFIG) $(ALL_WARNING)
PREFIX = /usr/local
LDLIBS = -lm
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man

all: awm
install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(MANDIR)/man1
	cp -f awm $(DESTDIR)$(BINDIR)
	cp -f awm.1 $(DESTDIR)$(MANDIR)/man1
	chmod 755 $(DESTDIR)$(BINDIR)/awm
	chmod 644 $(DESTDIR)$(MANDIR)/man1/awm.1
awm: awm.cpp
	$(CC) $(ALL) *.cpp -o awm
clean:
	rm -f awm *.o
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/awm
	rm -f $(DESTDIR)$(MANDIR)/man1/awm.1
.PHONY: all install uninstall clean
