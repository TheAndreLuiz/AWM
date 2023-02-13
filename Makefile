.POSIX:
CC = g++
PKG_CONFIG = `pkg-config xcb xcb-keysyms xcb-image gtkmm-4.0 --cflags --libs`
ALL_WARNING = -Wall -Wextra -pedantic
ALL_LDFLAGS = -lxcb -lxcb-keysyms -lxcb-image $(LDFLAGS)
ALL_CFLAGS = $(PKG_CONFIG) $(CPPFLAGS) $(CFLAGS) $(ALL_WARNING)
PREFIX = /usr/local
LDLIBS = -lm
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man

all: wm
install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(MANDIR)/man1
	cp -f wm $(DESTDIR)$(BINDIR)
	cp -f wm.1 $(DESTDIR)$(MANDIR)/man1
	chmod 755 $(DESTDIR)$(BINDIR)/wm
	chmod 644 $(DESTDIR)$(MANDIR)/man1/wm.1
wm: wm.o
	$(CC) $(ALL_LDFLAGS) -o wm wm.o $(LDLIBS)
wm.o: wm.cpp
clean:
	rm -f wm *.o
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/wm
	rm -f $(DESTDIR)$(MANDIR)/man1/wm.1
.PHONY: all install uninstall clean
