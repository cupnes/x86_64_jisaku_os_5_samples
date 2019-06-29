#include <lib.h>
#include <dotoon.h>

void init(struct env_info *ei);
void run(struct env_info *ei);
void update_env_info(struct env_info *ei);
void move_on(struct env_info *ei, unsigned char dir);
int is_movable(int x, int y, struct pixel *fg);

struct pixel cpu_color[NUM_CPUS];

int main(void)
{
	struct env_info ei;
	init(&ei);
	run(&ei);
	return 0;
}

void init(struct env_info *ei)
{
	unsigned char i;
	for (i = 0; i < NUM_CPUS; i++) {
		cpu_color[i].r = cpu_color[i].g = cpu_color[i].b = 255;
		cpu_color[i].a = 255;
	}
	cpu_color[0].g = cpu_color[1].r = cpu_color[2].b = 0;

	ei->prev_x = ei->prev_y = -1;
	ei->pause_counter = 0;

	ei->id = get_pnum();
	switch (ei->id) {
	case 0:
		ei->x = ei->y = 0;
		break;

	case 1:
		ei->x = SCREEN_WIDTH - 1;
		ei->y = 0;
		break;

	case 2:
		ei->x = SCREEN_WIDTH - 1;
		ei->y = SCREEN_HEIGHT - 1;
		break;

	case 3:
		ei->x = 0;
		ei->y = SCREEN_HEIGHT - 1;
		break;
	}

	draw_px(ei->x, ei->y, &cpu_color[ei->id]);
}

void run(struct env_info *ei)
{
	while (1) {
		update_env_info(ei);

		unsigned char dir = decide_next_dir(ei);

		move_on(ei, dir);
	}
}

void update_env_info(struct env_info *ei)
{
	ei->neighbor_point[UP] =
		is_movable(ei->x, ei->y - 1, &cpu_color[ei->id]);
	ei->neighbor_point[RIGHT] =
		is_movable(ei->x + 1, ei->y, &cpu_color[ei->id]);
	ei->neighbor_point[DOWN] =
		is_movable(ei->x, ei->y + 1, &cpu_color[ei->id]);
	ei->neighbor_point[LEFT] =
		is_movable(ei->x - 1, ei->y, &cpu_color[ei->id]);
}

void move_on(struct env_info *ei, unsigned char dir)
{
	ei->prev_x = ei->x;
	ei->prev_y = ei->y;

	if (ei->neighbor_point[dir] == 0)
		ei->pause_counter = 0;
	else
		ei->pause_counter += ei->neighbor_point[dir];

	switch (dir) {
	case UP:
		ei->y--;
		break;
	case RIGHT:
		ei->x++;
		break;
	case DOWN:
		ei->y++;
		break;
	case LEFT:
		ei->x--;
		break;
	}

	ei->dir = dir;

	draw_px(ei->x, ei->y, &cpu_color[ei->id]);

	volatile unsigned long long i;
	for (i = 0; i < ei->pause_counter; i++)
		CPU_PAUSE();
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

	return 1000000;
}
