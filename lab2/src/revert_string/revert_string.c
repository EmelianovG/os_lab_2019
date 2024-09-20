#include "revert_string.h"

void RevertString(char *str)
{
	int n = strlen(str);
	char* rev = (char*)malloc(n + 1);
	for (int i = 0; i < n; i++) {
		rev[n-i-1] = str[i];
	}
	rev[n] = '\0';
	strcpy(str, rev);
	free(rev);
}

