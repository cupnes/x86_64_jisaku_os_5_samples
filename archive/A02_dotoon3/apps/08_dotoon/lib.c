#include <lib.h>

/* 64bit unsignedの最大値0xffffffffffffffffは
 * 10進で18446744073709551615(20桁)なので'\0'含め21文字分のバッファで足りる */
#define MAX_STR_BUF	21

enum SYSCCALL_NO {
	SYSCALL_PUTC,
	SYSCALL_OPEN,
	SYSCALL_EXEC,
	SYSCALL_ENQ_TASK,
	SYSCALL_RCV_FRAME,
	SYSCALL_SND_FRAME,
	SYSCALL_EXEC_AP,
	SYSCALL_GET_PNUM,
	SYSCALL_GET_PX,
	SYSCALL_DRAW_PX,
	SYSCALL_SET_BG,
	SYSCALL_CLS,
	MAX_SYSCALL_NUM
};

unsigned long long syscall(
	unsigned long long syscall_id __attribute__((unused)),
	unsigned long long arg1 __attribute__((unused)),
	unsigned long long arg2 __attribute__((unused)),
	unsigned long long arg3 __attribute__((unused)))
{
	unsigned long long ret_val;

	asm volatile ("int $0x80\n"
		      "movq %%rax, %[ret_val]"
		      : [ret_val]"=r"(ret_val) :);

	return ret_val;
}

unsigned char get_pnum(void)
{
	return syscall(SYSCALL_GET_PNUM, 0, 0, 0);
}

inline unsigned long long random(void)
{
	unsigned long long rnd;
	asm volatile ("rdrand %[rnd]": [rnd]"=r"(rnd));
	return rnd;
}

void get_px(unsigned int x, unsigned int y, struct pixel *px)
{
	syscall(SYSCALL_GET_PX, x, y, (unsigned long long)px);
}

void draw_px(unsigned int x, unsigned int y, struct pixel *px)
{
	syscall(SYSCALL_DRAW_PX, x, y, (unsigned long long)px);
}

void set_bg(unsigned char r, unsigned char g, unsigned char b)
{
	syscall(SYSCALL_SET_BG, r, g, b);
}

void clear_screen(void)
{
	syscall(SYSCALL_CLS, 0, 0, 0);
}

void putc(char c)
{
	syscall(SYSCALL_PUTC, c, 0, 0);
}

void puts(char *s)
{
	while (*s != '\0')
		putc(*s++);
}

void puth(unsigned long long val, unsigned char num_digits)
{
	char str[MAX_STR_BUF];

	int i;
	for (i = num_digits - 1; i >= 0; i--) {
		unsigned char v = (unsigned char)(val & 0x0f);
		if (v < 0xa)
			str[i] = '0' + v;
		else
			str[i] = 'A' + (v - 0xa);
		val >>= 4;
	}
	str[num_digits] = '\0';

	puts(str);
}

struct file *open(char *file_name)
{
	return (struct file *)syscall(
		SYSCALL_OPEN, (unsigned long long)file_name, 0, 0);
}

void exec(struct file *file)
{
	syscall(SYSCALL_EXEC, (unsigned long long)file, 0, 0);
}

int exec_ap(struct file *file, unsigned char pnum)
{
	return (int)syscall(SYSCALL_EXEC_AP, (unsigned long long)file, pnum, 0);
}

void enq_task(struct file *file)
{
	syscall(SYSCALL_ENQ_TASK, (unsigned long long)file, 0, 0);
}

unsigned short receive_frame(void *buf)
{
	return syscall(SYSCALL_RCV_FRAME, (unsigned long long)buf, 0, 0);
}

unsigned char send_frame(void *buf, unsigned short len)
{
	return syscall(SYSCALL_SND_FRAME, (unsigned long long)buf, len, 0);
}
