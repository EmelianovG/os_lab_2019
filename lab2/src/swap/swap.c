#include "swap.h"

void Swap(char *left, char *right)
{
	char buf = *right;
	*right = *left;
	*left = buf;
}
