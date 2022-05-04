CCFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror
execs=pdr server client

all:$(execs)

#pdr
pdr: pdr.o utils_v1.o
	gcc $(CFLAGS) -o pdr pdr.o utils_v1.o

pdr.o: pdr.c
	gcc $(CFLAGS) -c pdr.c

#Server
server: server.o utils_v1.o
	gcc $(CFLAGS) -o server server.o utils_v1.o

server.o: server.c virement.h
	gcc $(CFLAGS) -c server.c

#Client
client: client.o utils_v1.o
	gcc $(CFLAGS) -o client client.o utils_v1.o

client.o: client.c virement.h
	gcc $(CFLAGS) -c client.c

#utils
utils_v1.o: utils_v1.c utils_v1.h
	gcc $(CFLAGS) -c utils_v1.c


clean:
	rm *.o
	rm $(execs)