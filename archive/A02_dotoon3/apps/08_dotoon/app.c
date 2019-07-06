#include <lib.h>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define MAP_FILENAME	"map.bin"

#define TITLE_R	1
#define TITLE_G	1
#define TITLE_B	1

int main(void)
{
	struct file *map_file = open(MAP_FILENAME);
	unsigned char *map = map_file->data;

	struct pixel fg = {TITLE_R, TITLE_G, TITLE_B, 255};
	struct pixel bg = {0, 0, 0, 255};
	unsigned int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++)
		for (x = 0; x < SCREEN_WIDTH; x++)
			if (*(map + (SCREEN_WIDTH * y) + x))
				draw_px(x, y, &fg);
			else
				draw_px(x, y, &bg);

	struct file *cpu = open("dotoon_cpu");
	exec_ap(cpu, 1);
	exec_ap(cpu, 2);
	exec_ap(cpu, 3);
	exec(cpu);

	return 0;
}
