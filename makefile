# ./PHONY:clean

all:
	gcc -c src/nccshell.c -Iincludes -o bin/nccshell.o
	gcc -c src/getconf.c -Iincludes -o bin/getconf.o
	gcc -c src/connection.c -Iincludes -o bin/connection.o
	gcc -c src/threads.c -Iincludes -o bin/threads.o
	gcc -c src/user_list.c -Iincludes -o bin/user_list.o
	gcc -c udp_chat.c -Iincludes -o bin/udp_chat.o
	gcc bin/getconf.o bin/nccshell.o bin/connection.o bin/threads.o bin/udp_chat.o bin/user_list.o -Iincludes -o ./chat.out -pthread

clean:
	rm bin/*

other:
	gcc -c src/addrlist.c -Iincludes -o bin/addrlist
	gcc -c src/testlist.c -Iincludes -o bin/testlist
