# ./PHONY:clean

all:
	g++ -c src/nccshell.cpp -Iincludes -o bin/nccshell.o
	g++ -c src/getconf.cpp -Iincludes -o bin/getconf.o
	g++ -c src/connection.cpp -Iincludes -o bin/connection.o
	g++ -c src/threads.cpp -Iincludes -o bin/threads.o
	g++ -c src/user_list.cpp -Iincludes -o bin/user_list.o
	g++ -c udp_chat.cpp -Iincludes -o bin/udp_chat.o
	g++ bin/getconf.o bin/nccshell.o bin/connection.o bin/threads.o bin/udp_chat.o bin/user_list.o -Iincludes -o ./chat.out -pthread

clean:
	rm bin/*

other:
	g++ -c src/addrlist.cpp -Iincludes -o bin/addrlist
	g++ -c src/testlist.cpp -Iincludes -o bin/testlist
