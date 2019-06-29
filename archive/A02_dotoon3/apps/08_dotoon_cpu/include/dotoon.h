#pragma once

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define BG_R	0
#define BG_G	0
#define BG_B	0
#define NUM_CPUS	4

enum {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	NUM_DIRS
};

struct env_info {
	unsigned char id;
	int x, y;
	int prev_x, prev_y;
	unsigned char dir;
	int neighbor_point[NUM_DIRS];
	unsigned long long pause_counter;
};

unsigned char decide_next_dir(struct env_info *ei);
