CC=gcc
CFLAGS=-std=c99 -Iinclude
DEPS = .h

hc_check: hc_check.c src/hclib.c
	$(CC) -o $@ $^ $(CFLAGS)
