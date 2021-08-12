FLAGS = -std=c11 -Werror -lm $(shell pkg-config --cflags --libs check)

# --- Before --- #
lib:
	gcc -fPIC -c ./lib/tcpsock.c
	mv ./tcpsock.o ./lib/tcpsock.o
	gcc -shared -o ./lib/libtcpsock.so ./lib/tcpsock.o

	gcc -fPIC -c ./lib/dplist.c
	mv ./dplist.o ./lib/dplist.o
	gcc -shared -o ./lib/libdplist.so ./lib/dplist.o

# --- Main --- #
main:
	mkdir ./build -p
	gcc main.c connmgr.c datamgr.c sensor_db.c sbuffer.c -lsqlite3 -lpthread -ldplist -ltcpsock -L./lib -Wl,-rpath=./lib -o ./build/main -Wall -Werror -DTIMEOUT=5 -DSET_MIN_TEMP=-10 -DSET_MAX_TEMP=50
	./build/main 1234

debug-main:
	mkdir ./build -p
	gcc main.c connmgr.c datamgr.c sensor_db.c sbuffer.c -lsqlite3 -lpthread -ldplist -ltcpsock -L./lib -Wl,-rpath=./lib -o ./build/main -Wall -Werror -DTIMEOUT=5 -DSET_MIN_TEMP=-10 -DSET_MAX_TEMP=50 -DDEBUG
	./build/main 1234

valgrind-main:
	mkdir ./build -p
	gcc main.c connmgr.c datamgr.c sensor_db.c sbuffer.c -lsqlite3 -lpthread -ldplist -ltcpsock -L./lib -Wl,-rpath=./lib -o ./build/main -Wall -Werror -DTIMEOUT=5 -DSET_MIN_TEMP=-10 -DSET_MAX_TEMP=50 -DDEBUG
	valgrind --leak-check=full --show-leak-kinds=all ./build/main

# --- Tests --- #

datamgr-test:
# file creator
	mkdir ./build -p
	gcc file_creator.c -DDEBUG -o ./build/file_creator
	./build/file_creator
# datamgr
	gcc ./Tests/main_files/datamgr_main.c datamgr.c -ldplist -L./lib -Wl,-rpath=./lib -o ./build/datamgr_test $(FLAGS) -D SET_MAX_TEMP=50 -D SET_MIN_TEMP=-10
	./build/datamgr_test

sensor-db-test: # add more data in db for this to work better
	mkdir ./build -p
	gcc -Wall ./Tests/main_files/sensor_db_main.c sensor_db.c -lsqlite3 -o ./build/sensor_db_test $(FLAGS)
	./build/sensor_db_test

connmgr-test:
	mkdir ./build -p
	gcc ./Tests/main_files/connmgr_main.c connmgr.c -ldplist -ltcpsock -L./lib -Wl,-rpath=./lib -o build/connmgr_test -Wall -Werror -DTIMEOUT=5
	./build/connmgr_test

