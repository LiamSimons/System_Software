# Create your own targets that build the dynamic library for the list and then compile the application


# the files for ex3 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab5_ex3.zip datamgr.c datamgr.h config.h lib/dplist.c lib/dplist.h

datamgr: dplist datamgr.c
	@echo -e '\n************************************'
	@echo -e '*********compiling for test*********'
	@echo -e '************************************'
	mkdir -p build
	gcc -Wall -Werror -o ./build/datamgr -L./lib/ -Wl,-rpath=./lib/ -ldplist datamgr.c -D SET_MAX_TEMP=30 -D SET_MIN_TEMP=-5

dplist: lib/dplist.c
	gcc -fPIC -c ./lib/dplist.c
	mv ./dplist.o ./lib/dplist.o
	gcc -shared -o ./lib/libdplist.so ./lib/dplist.o
	#here add type de comand to make de dynamic library.

test:
	@echo -e '\n************************************'
	@echo -e '*********compiling for test*********'
	@echo -e '************************************'
	gcc -o test.out test.c datamgr.c ./lib/dplist.c -D SET_MAX_TEMP=27.96 -DSET_MIN_TEMP=14.01 -Wall -Werror
	./test.out

debug:
	gcc datamgr.c test.c ./lib/dplist.c -o test.out -Wall -Werror -std=c11 -g -D SET_MAX_TEMP=27.96 -DSET_MIN_TEMP=14.01
	gdb test.out