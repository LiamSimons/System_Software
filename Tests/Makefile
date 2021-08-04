test:	mkdir
# Step 1: compiling with position independent code
	gcc -c -Wall -Werror -fPIC test_lib.c -o ./build/test_lib.o
# Step 2: creating a shared library from an object file
	gcc -shared -o ./build/libtest.so ./build/test_lib.o
# Step 3: linking with a shared library
	gcc -L./build -Wall -o test_lib test_main.c -ltest
# Step 4: making the library available at runtime
# echo $LD_LIBRARY_PATH    -> reveals there is nothing in there
# export LD_LIBRARY_PATH=/home/liam/Desktop/Sysprog/lab5/Tests/build:$LD_LIBRARY_PATH

mkdir:
	mkdir -p build