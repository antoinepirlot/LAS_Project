CCFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror
execs= server

all:$(execs)

#Server
server: server.o utils_v1.o
	cc $(CFLAGS) -o server server.o utils_v1.o

server.o: server.c
	cc $(CFLAGS) -c server.c

utils_v1.o: utils_v1.c utils_v1.h
	cc $(CFLAGS) -c utils_v1.c

clean:
	rm *.o
	rm $(execs)