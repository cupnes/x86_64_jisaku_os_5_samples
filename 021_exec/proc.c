#include <fs.h>
#include <fbcon.h>

void exec(struct file *f)
{
	void *start_addr = (void *)f->data;
	puth((unsigned long long)start_addr, 16);
	asm volatile ("call *%[start_addr]" :: [start_addr]"r"(start_addr));
}
