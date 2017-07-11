server:	server.c
	gcc -no-pie -fno-stack-protector server.c -g -o level2 /usr/lib/x86_64-linux-gnu/libuv.a -pthread -lm
	cp level2 ../server_fork/level2/
all:
	gcc -no-pie -fno-stack-protector server.c -g -o level2 /usr/lib/x86_64-linux-gnu/libuv.a -pthread -lm
	cp level2 ../server_fork/level2/
clean:
	rm level2
	rm ../server_fork/level2/level2
