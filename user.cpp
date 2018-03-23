#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int a;
	while(scanf("%d", &a) != EOF)
		printf("%d\n", a);
	return 0;
}
