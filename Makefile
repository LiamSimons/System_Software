# Create your own targets that compile the application

FLAGS = -std=c11 -Werror -lm $(shell pkg-config --cflags --libs check)

test1:
	gcc -Wall main.c sensor_db.c -lsqlite3 -o ./build/test1 $(FLAGS)
	./build/test1

test2:
	gcc -Wall main.c sensor_db.c sqlite3.c -pthread -ldl


# the files for ex2 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab6_ex2.zip sensor_db.c config.h



# test: lib datamgr.c datamgr.h build/
# 		@echo -e '\n************************************'
# 		@echo -e '*********compiling for datamgr_test.c*********'
# 		@echo -e '************************************'
# 		gcc datamgr_test.c datamgr.c -ldplist -L./build/lib -Wl,-rpath=./lib -o ./build/test $(FLAGS) -D SET_MAX_TEMP=50 -D SET_MIN_TEMP=-10
# 		@echo -e '\n************************************'
# 		@echo -e '*********running datamgr_test.c*********'
# 		@echo -e '************************************'		
# 		./build/test

# lib: ./lib/dplist.c ./lib/dplist.h build/
# 	gcc -fPIC -c ./lib/dplist.c -o ./build/lib/dplist.o
# 	gcc -shared -o ./build/lib/libdplist.so ./build/lib/dplist.o

# gitupdatedplist:
# 	git show origin/lab4:ex3/dplist.c > lib/dplist.c

# file_creator: #build
# 	mkdir -p build/lib

# clean:
# 	rm -rf ./build

# valgrind:
# 	valgrind ./build/test

# debug:
# 	gcc datamgr.c datamgr_test.c ./lib/dplist.c -o datamgr_test.out -Wall -Werror -std=c11 -g -DSET_MAX_TEMP=27.96 -DSET_MIN_TEMP=14.01
# 	gdb ./datamgr_test.out -tui

