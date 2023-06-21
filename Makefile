CC = gcc
OPT = -Wall

all: sush genkey

sush:
	${CC} sush.c ${OPT} -lmicrohttpd -lsqlite3 -o sush

genkey:
	${CC} genkey.c ${OPT} -lsqlite3 -o genkey

clean:
	rm sush
	rm genkey

