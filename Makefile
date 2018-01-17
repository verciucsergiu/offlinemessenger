sv:
	gcc -pthread -o server.o server/server.c server/db_handler/db_handler.c server/login/login.c helper/helpers.c `mysql_config --cflags --libs` -w
cl:
	gcc -o client.o client/client.c helper/helpers.c -lncurses -pthread -w
runcl:
	./client.o 127.0.0.1
runsv:
	./server.o