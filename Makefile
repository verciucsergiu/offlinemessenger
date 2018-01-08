sv:
	gcc -pthread server/server.c -o server.o -w
cl:
	gcc -o client.o client/client.c helper/helpers.c -lncurses -pthread -w
runcl:
	./client.o 127.0.0.1
runsv:
	./server.o