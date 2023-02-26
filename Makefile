.POSIX:
CC = g++
PKG_CONFIG = `pkg-config --cflags --libs xcb xcb-icccm xcb-keysyms xcb-image gtkmm-4.0 glib-2.0`
ALL_WARNING = -Wall -Wextra -pedantic
ALL = $(PKG_CONFIG) $(ALL_WARNING)
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
wm: wm.cpp
	$(CC) $(ALL) *.cpp -o wm
clean:
	rm -f wm *.o
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/wm
	rm -f $(DESTDIR)$(MANDIR)/man1/wm.1
.PHONY: all install uninstall clean
