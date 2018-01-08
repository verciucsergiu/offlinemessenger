sv:
	gcc -pthread server/server.c -o server.o
cl:
	gcc client/client.c -o client.o -lncurses -pthread
runcl:
	./client.o 127.0.0.1
runsv:
	./server.o