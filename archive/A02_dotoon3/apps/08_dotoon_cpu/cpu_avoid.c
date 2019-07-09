#include <lib.h>
#include <dotoon.h>

unsigned char decide_next_dir(struct env_info *ei)
{
	unsigned char dir;
	do {
		dir = random() % NUM_DIRS;
	} while (ei->neighbor_point[dir] < 0);
	return dir;
}
