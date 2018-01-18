sv:
	gcc -pthread -o server.o server/server.c server/db_handler/db_handler.c server/login/login.c server/register/register.c helper/json.c server/message_handler/message_handler.c `mysql_config --cflags --libs` -w
cl:
	gcc -o client.o client/client.c helper/json.c -lncurses -pthread -w
runcl:
	./client.o 127.0.0.1
runsv:
	./server.o