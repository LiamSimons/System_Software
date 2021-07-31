HEADERS=dplist.h
SOURCE=dplist.c
EXE=build/dplist

FLAGS = -Wall -std=c11 -Werror -lm $(shell pkg-config --cflags --libs check)

test :
	@echo -e '\n*******************************'
	@echo -e '*** Compiling for UNIT TEST ***'
	@echo -e '*******************************'
	mkdir -p build
	gcc dplist.c dplist_test.c -o build/dplist_test $(FLAGS)
	@echo -e '\n*************************'
	@echo -e '*** Running UNIT TEST ***'
	@echo -e '*************************'
	./build/dplist_test

test_debug:
	@echo -e '\n*******************************'
	@echo -e '*** Compiling for UNIT TEST ***'
	@echo -e '*******************************'
	mkdir -p build
	gcc -D DEBUG dplist.c dplist_test.c -o build/dplist_test $(FLAGS)
	@echo -e '\n*************************'
	@echo -e '*** Running UNIT TEST ***'
	@echo -e '*************************'
	./build/dplist_test

debug1 :
	@echo -e '\n*******************************'
	@echo -e '*** Compiling for DEBUG ***'
	@echo -e '*******************************'
	mkdir -p build
	gcc dplist.c dplist_test.c -o build/dplist_test $(FLAGS) -g
	@echo -e '\n*************************'
	@echo -e '*** Running UNIT TEST DEBUG ***'
	@echo -e '*************************'
	CK_FORK=no gdb -tui ./build/dplist_test

valgrind:
	@echo -e '\n*******************************'
	@echo -e '*** Compiling for DEBUG ***'
	@echo -e '*******************************'
	mkdir -p build
	gcc dplist.c dplist_test.c -o build/dplist_test $(FLAGS) -g
	@echo -e '\n*************************'
	@echo -e '*** Running UNIT TEST DEBUG ***'
	@echo -e '*************************'
	CK_FORK=0 valgrind --leak-check=full ./build/dplist_test
	
.PHONY : clean check

check : $(SOURCE) $(HEADERS)
	@echo "Running cppcheck :"
	cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 $(SOURCE) $(HEADERS)

clean:
	rm -f *.o $(EXE) $(EXE)_test *~

