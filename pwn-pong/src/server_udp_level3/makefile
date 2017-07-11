server:	server.c
	gcc server.c -g -o level3 /usr/local/lib/libuv.a -pthread -lm
	cp level3 ../server_fork/level3/
all:
	gcc server.c -g -o level3 /usr/local/lib/libuv.a -pthread -lm
	cp level3 ../server_fork/level3/
clean:
	rm level3
	rm ../server_fork/level3/level3