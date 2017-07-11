server:	server.c
	gcc server.c -no-pie -o level1 /usr/lib/x86_64-linux-gnu/libuv.a -pthread -lm -mpreferred-stack-boundary=4 -fno-stack-protector
	cp level1 ../server_fork/level1/
all:
	gcc server.c -no-pie -o level1 /usr/lib/x86_64-linux-gnu/libuv.a -pthread -lm -mpreferred-stack-boundary=4 -fno-stack-protector
	cp level1 ../server_fork/level1/
clean:
	rm level1
	rm ../server_fork/level1/level1
