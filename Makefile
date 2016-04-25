CC=gcc
CFLAGS=-std=c99 -Iinclude  -lm
DEPS = .h

all: hc_check hc_to_dot hc_to_concorde hc_solve

hc_check: hc_check.c src/hclib.c
	$(CC) -o $@ $^ $(CFLAGS)

hc_to_dot: hc_to_dot.c src/hclib.c
	$(CC) -o $@ $^ $(CFLAGS)

hc_to_concorde: hc_to_concorde.c src/hclib.c
	$(CC) -o $@ $^ $(CFLAGS)

hc_solve: hc_solve.c src/hclib.c src/solvers/*.c src/solvers/slh/*.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f hc_check hc_to_dot hc_to_concorde hc_solve
