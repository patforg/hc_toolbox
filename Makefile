CC=gcc
CFLAGS=-std=c99 -Iinclude 
DEPS = .h

hc_check: hc_check.c src/hclib.c
	$(CC) -o $@ $^ $(CFLAGS)

hc_to_dot: hc_to_dot.c src/hclib.c
		$(CC) -o $@ $^ $(CFLAGS)

hc_solve: hc_solve.c src/hclib.c src/solvers/*.c
		$(CC) -o $@ $^ $(CFLAGS)
