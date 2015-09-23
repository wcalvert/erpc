all: client

client:
	gcc -o client client.c ../common/rpcproto.pb.c ../common/common.c ../common/pb_decode.c ../common/pb_encode.c ../common/pb_common.c -I. -I../common -std=c99 -g -O0 -lusb-1.0

clean:
	rm -f client