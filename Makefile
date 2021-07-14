# if you created a subfolder for every exercise this target will
# build and execute the main.c file in the ex3 folder
ex1:
	mkdir -p build
	gcc -Wall -Werror -o ./build/ex1 ex1.c
	./build/ex1

ex1_idea:
	mkdir -p build
	gcc -Wall -Werror -o ./build/ex1_idea ex1_idea.c
	./build/ex1_idea

main:
	mkdir -p build
	gcc -Wall -Werror -o ./build/main main.c
	./build/main

swap:
	mkdir -p build
	gcc -Wall -Werror -o ./build/swap_pointers swap_pointers.c
	./build/swap_pointers

# the main file in the ex3 folder will be ziped and is then ready to
# be submitted to labtools.groept.be
zip:
	zip ../lab2.zip main.c