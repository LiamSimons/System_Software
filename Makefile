# if you created a subfolder for every exercise this target will
# build and execute the main.c file in the ex3 folder
ex1:
	mkdir -p build
	gcc -Wall -Werror -o ./build/ex1 main.c
	./build/ex1

# the main file in the ex3 folder will be ziped and is then ready to
# be submitted to labtools.groept.be
zip:
	zip ../lab2.zip main.c