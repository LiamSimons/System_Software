# if you created a subfolder for every exercise this target will
# build and execute the main.c file in the ex3 folder
ex1:
	mkdir -p build
	gcc -Wall -Werror -o ./build/ex1 ex1.c
	./build/ex1

main:
	mkdir -p build
	gcc -Wall -Werror -o ./build/main main.c ma_malloc.c
	./build/main

debug:
	mkdir -p build
	gcc -g -Wall -Werror -o ./build/main main.c ma_malloc.c
	gdb -tui ./build/main


# the main file in the ex3 folder will be ziped and is then ready to
# be submitted to labtools.groept.be
zip:
	zip ../lab3.zip ma_malloc.c ma_malloc.h