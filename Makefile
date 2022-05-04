CCFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror
execs= pdr

all:$(execs)

pdr: pdr.c utils_v1.o
	gcc $(CFLAGS) -o pdr pdr.c utils_v1.o

clean:
	rm *.o
	rm $(execs)