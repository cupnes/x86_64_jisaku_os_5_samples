#include <lib.h>

#define MAX_EXEC_COUNT	2

int main(void)
{
	unsigned char i;
	for (i = 0; i < MAX_EXEC_COUNT;) {
		if (exec_ap(open("puta"), 1) == 0)
			i++;
	}

	return 0;
}
