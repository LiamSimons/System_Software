#include <stdio.h>

int main(){
	char* x, y;
	*x = 'a';
	printf("TYPEOF :\tX = %p\tY = %d\n", x, y);
	char* a, *b;
	printf("TYPEOF :\tA = %p\tY = %p\n", a, b);
}

// CONLCUSION:
/*
	To declare multiple pointers in one line, add * before each variable name
	otherwise the second one will be of the declared type and not a pointer to it.
*/