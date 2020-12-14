# Create your own targets that compile the application


# the files for ex2 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab7_ex2.zip connmgr.c connmgr.h config.h lib/dplist.c lib/dplist.h lib/tcpsock.c lib/tcpsock.h

server:
	gcc main.c connmgr.c lib/dplist.c lib/tcpsock.c -o server -Wall -Werror -DTIMEOUT=5

client:
	gcc sensor_node.c lib/tcpsock.c -o client -Wall -Werror

lin:
	gcc -fPIC -c ./lib/tcpsock.c
	mv ./tcpsock.o ./lib/tcpsock.o
	gcc -shared -o ./lib/libtcpsock.so ./lib/tcpsock.o

	gcc -fPIC -c ./lib/dplist.c
	mv ./dplist.o ./lib/dplist.o
	gcc -shared -o ./lib/libdplist.so ./lib/dplist.o

debug:
	gcc test.c lib/tcpsock.c lib/dplist.c -o debug -g
	gdb debug 5678

valgrind:
	gcc main.c connmgr.c lib/dplist.c lib/tcpsock.c -o server -Wall -Werror -DTIMEOUT=5 -g
	valgrind --leak-check=full --show-leak-kinds=all ./server 5678