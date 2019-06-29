#include <lib.h>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define BG_R	0
#define BG_G	0
#define BG_B	0

void init(unsigned int *px, unsigned int *py, struct pixel *fg);
void run(unsigned int *px, unsigned int *py, struct pixel *fg,
	 unsigned long long *pause_counter);
int search(unsigned int *px, unsigned int *py, struct pixel *fg,
	   unsigned long long *pause_counter);
int is_movable(int x, int y, struct pixel *fg);

enum {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	NUM_DIRS
};

int main(void)
{
	unsigned int px, py;
	struct pixel fg;
	unsigned long long pause_counter = 0;
	fg.a = 255;
	init(&px, &py, &fg);
	run(&px, &py, &fg, &pause_counter);
	return 0;
}

void init(unsigned int *px, unsigned int *py, struct pixel *fg)
{
	switch (get_pnum()) {
	case 0:
		*px = *py = 0;
		fg->r = fg->b = 255;
		fg->g = 0;
		break;

	case 1:
		*px = SCREEN_WIDTH - 1;
		*py = 0;
		fg->b = fg->g = 255;
		fg->r = 0;
		break;

	case 2:
		*px = SCREEN_WIDTH - 1;
		*py = SCREEN_HEIGHT - 1;
		fg->r = fg->g = 255;
		fg->b = 0;
		break;

	case 3:
		*px = 0;
		*py = SCREEN_HEIGHT - 1;
		fg->r = fg->g = fg->b = 255;
		break;
	}
	draw_px(*px, *py, fg);
}

void run(unsigned int *px, unsigned int *py, struct pixel *fg,
	 unsigned long long *pause_counter)
{
	while (1) {
		search(px, py, fg, pause_counter);

		draw_px(*px, *py, fg);

		unsigned long long i;
		for (i = 0; i < *pause_counter; i++)
			CPU_PAUSE();
	}
}

int search(unsigned int *px, unsigned int *py, struct pixel *fg,
	   unsigned long long *pause_counter)
{
	int tx = *px;
	int ty = *py;
	int neighbor_point[NUM_DIRS];
	neighbor_point[UP] = is_movable(tx, ty - 1, fg);
	neighbor_point[RIGHT] = is_movable(tx + 1, ty, fg);
	neighbor_point[DOWN] = is_movable(tx, ty + 1, fg);
	neighbor_point[LEFT] = is_movable(tx - 1, ty, fg);

	unsigned char nd;
	do {
		nd = random() % NUM_DIRS;
	} while (neighbor_point[nd] < 0);

	if (neighbor_point[nd] == 0)
		*pause_counter = 0;
	else
		*pause_counter += neighbor_point[nd];

	switch (nd) {
	case UP:
		(*py)--;
		break;
	case RIGHT:
		(*px)++;
		break;
	case DOWN:
		(*py)++;
		break;
	case LEFT:
		(*px)--;
		break;
	}
	return nd;
}

int is_movable(int x, int y, struct pixel *fg)
{
	if ((x < 0) || (x >= SCREEN_WIDTH) || (y < 0) || (y >= SCREEN_HEIGHT))
		return -1;

	struct pixel px;
	get_px(x, y, &px);
	if (((px.r == BG_R) && (px.g == BG_G) && (px.b == BG_B))
	    || ((px.r == fg->r) && (px.g == fg->g) && (px.b == fg->b)))
		return 0;

	return 100000;
}
