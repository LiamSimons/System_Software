lib:
	gcc -fPIC -c ./lib/tcpsock.c
	mv ./tcpsock.o ./lib/tcpsock.o
	gcc -shared -o ./lib/libtcpsock.so ./lib/tcpsock.o

	gcc -fPIC -c ./lib/dplist.c
	mv ./dplist.o ./lib/dplist.o
	gcc -shared -o ./lib/libdplist.so ./lib/dplist.o

dplist-test:
	gcc dplist-test.c -ldplist -L./lib -Wl,-rpath=./lib -o dplist-test -Wall -Werror
	./dplist-test

dplist-test-valgrind:
	gcc dplist-test.c -ldplist -L./lib -Wl,-rpath=./lib -o dplist-test -Wall -Werror
	valgrind --leak-check=full --show-leak-kinds=all ./dplist-test

test-server:
	gcc test_server.c -ltcpsock -L./lib -Wl,-rpath=./lib -o test-server -Wall -Werror -DTIMEOUT=5

client:
	gcc sensor_node.c -ltcpsock -L./lib -Wl,-rpath=./lib -o client -Wall -Werror

main:
	gcc main.c connmgr.c -ldplist -ltcpsock -L./lib -Wl,-rpath=./lib -o main -Wall -Werror -DTIMEOUT=5
	./main