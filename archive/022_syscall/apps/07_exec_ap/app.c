#include <lib.h>

int main(void)
{
	putc('0');

	exec_ap(open("test1"), 1);
	exec_ap(open("test2"), 2);
	exec_ap(open("test3"), 3);

	while (1);

	return 0;
}
