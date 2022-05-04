

CCFLAGS = -std=c11 -pedantic -Wall -Wvla -Werror -D_POSIX_C_SOURCE -D_DEFAULT_SOURCE -g

all: client

client: client.o utils_v1.o
	gcc $(CCFLAGS) -o client client.o utils_v1.o

client.o: client.c utils_v1.h
	gcc $(CCFLAGS) -c client.c

utils_v1.o: utils_v1.c utils_v1.h
	gcc $(CCFLAGS)s -c utils_v1.c

clean:
	rm -f *.o
	rm -f err.txt
	rm -f client
