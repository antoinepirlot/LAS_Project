CCFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror
execs= maint

all:$(execs)

maint: maint.o
	cc $(CCFLAGS) -o maint maint.o utils_v1.o

maint.o: maint.c utils_v1.o
	cc $(CCFLAGS) -c maint.c

utils_v1.o: utils_v1.c utils_v1.h
	$(CC) $(CCFLAGS) -c utils_v1.c
clean:
	rm *.o
	rm $(execs)