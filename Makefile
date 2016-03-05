CC=gcc
CFLAGS=-std=c99
DEPS = .h

hc_check: hc_check.c lib/hclib.c
	$(CC) -o $@ $^ $(CFLAGS)
