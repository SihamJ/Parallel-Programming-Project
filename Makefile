###############################################################################
CC ?= gcc
CPPFLAGS += -I/usr/X11/include

# CFLAGS
CFLAGS ?= -g -O3 -march=native -flto -fopenmp
override CFLAGS += -std=gnu11 -pedantic
override CFLAGS += -Wall -Wextra
override CFLAGS += -Wformat=2 -Winit-self -Wswitch-default -Wunused-parameter \
		-Wuninitialized -Wshadow -Wunsafe-loop-optimizations -Wjump-misses-init \
		-Wlogical-op -Wredundant-decls -Wnested-externs

# LDFLAGS
LDFLAGS ?= $(CFLAGS)
LDLIBS ?= -lm
X11LIBS ?= -L/usr/X11/lib -lX11
###############################################################################
ALL = problem.o solution_check.o solution_io.o solve.o util.o street_table.o
#ALL = problem.o solution_check.o solution_io.o alt.o util.o

all: solver checker

iotest: problem.o solution_io.o util.o iotest.o street_table.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

solver: $(ALL) solver.o consoleprint.o street_table.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

checker: problem.o solution_io.o solution_check.o util.o checker.o consoleprint.o street_table.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# tous les .o existants dépendent de tous les .h et du Makefile:
$(ALL) $(wildcard *.o): $(wildcard *.h) Makefile

clean:
	rm -f *.o solver checker iotest gsolver

tar:
	tar czf out/archive.tar.gz *.[ch] Makefile
