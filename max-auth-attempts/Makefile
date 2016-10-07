CC = gcc
CFLAGS = -g -Wall

all: test-max-auth-attempts

test-max-auth-attempts: main.o
	$(CC) $(CFLAGS) $^ -ljansson -lssh2 -lz -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

install:

	$(chmod 755 test-max-auth-attempts)
	$(mkdir -p /etc/honeyssh-test)
	$(cp config.json /etc/honeyssh-test/config.json)

clean:
	\/bin/rm -f *.o
