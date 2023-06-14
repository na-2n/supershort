CC = gcc

sush:
	${CC} sush.c -lmicrohttpd -lsqlite3 -o sush

clean:
	rm -v sush

