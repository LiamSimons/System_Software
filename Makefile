FLAGS = -std=c11 -Werror -lm $(shell pkg-config --cflags --libs check)

datamgr: lib datamgr.c datamgr.h build/
		gcc datamgr.c -ldplist -L./build/lib -Wl,-rpath=./lib -o ./build/datamgr -D SET_MAX_TEMP=30 -D SET_MIN_TEMP=-5

test: lib datamgr.c datamgr.h build/
		@echo -e '\n************************************'
		@echo -e '*********compiling for datamgr_test.c*********'
		@echo -e '************************************'
		gcc datamgr_test.c datamgr.c -ldplist -L./build/lib -Wl,-rpath=./lib -o ./build/test $(FLAGS) -D SET_MAX_TEMP=30 -D SET_MIN_TEMP=-5
		@echo -e '\n************************************'
		@echo -e '*********running datamgr_test.c*********'
		@echo -e '************************************'		
		./build/test

lib: ./lib/dplist.c ./lib/dplist.h build/
		gcc -fPIC -c ./lib/dplist.c -o ./build/lib/dplist.o
		gcc -shared -o ./build/lib/libdplist.so ./build/lib/dplist.o

gitupdatedplist:
		git show origin/lab4:ex3/dplist.c > lib/dplist.c

zip:
		zip lab5_ex3.zip datamgr.c datamgr.h config.h lib/dplist.c lib/dplist.h

file_creator: #build
		mkdir -p build/lib

clean:
		rm -rf ./build

valgrind:
		valgrind ./build/test

debug:
		gcc datamgr.c datamgr_test.c ./lib/dplist.c -o datamgr_test.out -Wall -Werror -std=c11 -g -DSET_MAX_TEMP=27.96 -DSET_MIN_TEMP=14.01
		gdb ./datamgr_test.out -tui