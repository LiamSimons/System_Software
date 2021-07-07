# if you created a subfolder for every exercise this target will
# build and execute the main.c file in the ex3 folder
ex3:
	mkdir -p build
	gcc -Wall -Werror -o ./build/ex3 main.c ma_malloc.c
	./build/ex3

ex1:
	mkdir -p build
	gcc -Wall -Werror -o ./build/ex1 main.c 
	./build/ex1
	
# the main file in the ex3 folder will be ziped and is then ready to
# be submitted to labtools.groept.be
zip:
	#cd ex3
	zip ../lab3.zip ma_malloc.c ma_malloc.h
	cd ..
