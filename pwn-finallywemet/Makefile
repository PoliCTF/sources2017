all: proto
	gcc -static -fPIE met.c met.pb-c.c -o met -fstack-check -L libcgroup-0.41/src/.libs/ -L protobuf-c/protobuf-c/.libs -lcgroup -lpthread -lprotobuf-c
proto: 
	protoc-c --c_out=. met.proto

clean:
	rm met met.pb-c.c met.pb-c.h
