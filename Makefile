CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Wno-unused-function
LDFLAGS=#-lpthread

PROJECT=$(shell basename $(shell pwd))
VERSION=1.0
TARNAME=$(PROJECT)-$(VERSION).tar.xz

TARGETS=\
	shttpd

.PHONY: all clean dist distclean
all: $(TARGETS)

clean:
	@echo -n "Cleaning project $(PROJECT)... "
	@rm -f *.c.o $(TARGETS) && echo "done!" || echo "failed!"

dist: distclean
	@echo "Building distribution..."
	@(cd .. && tar cv --exclude=.git ./$(PROJECT) | xz -9 > $(TARNAME)) && echo "done!" || echo "failed!"

distclean:
	@echo "Cleaning distribution..."
	@($(MAKE) clean && rm -f *.bak) && echo "done!" || echo "failed!"

shttpd: shttpd.c.o abuffer.c.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

