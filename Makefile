CC=gcc
CFLAGS=-std=c99
DEPS = .h

hc_check: src/hc_check.c src/lib/hclib.c
	$(CC) -o $@ $^ $(CFLAGS)
