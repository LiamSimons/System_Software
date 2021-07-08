
#include <stdio.h>
#include <ctype.h>
#include <string.h>

//void upper_string(char [])

int main()
{
	int MAX = 10;
	char first[MAX + 1], second[MAX + 1],
	/* str[2*MAX + 1],*/ name[2*MAX + 1];
	printf("Enter your first name: ");
	scanf("%s", first);
	printf("Enter your second name: ");
	scanf("%s", second);

//	printf("%s %s (1)\n", first, second);
//	char *strcpy(char *str, const char *second);
//	printf("%s (2)\n", &str);
	strcat(name, first);
	strcat(name, second);

	printf(" %s\n %s\n %s\n", first, second, name);
	return 0;
}

