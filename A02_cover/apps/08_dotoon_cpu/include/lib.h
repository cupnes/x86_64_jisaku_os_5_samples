#pragma once

#define FILE_NAME_LEN	28
struct __attribute__((packed)) file {
	char name[FILE_NAME_LEN];
	unsigned int size;
	unsigned char data[0];
};

struct __attribute__((packed)) pixel {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
};

unsigned char get_pnum(void);
unsigned long long random(void);
void get_px(unsigned int x, unsigned int y, struct pixel *px);
void draw_px(unsigned int x, unsigned int y, struct pixel *px);
void set_bg(unsigned char r, unsigned char g, unsigned char b);
void clear_screen(void);
void putc(char c);
void puts(char *s);
void puth(unsigned long long val, unsigned char num_digits);
struct file *open(char *file_name);
void exec(struct file *file);
int exec_ap(struct file *file, unsigned char pnum);
void enq_task(struct file *file);
unsigned short receive_frame(void *buf);
unsigned char send_frame(void *buf, unsigned short len);
