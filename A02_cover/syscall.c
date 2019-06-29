#include <ap.h>
#include <intr.h>
#include <apic.h>
#include <pic.h>
#include <fb.h>
#include <fbcon.h>
#include <fs.h>
#include <proc.h>
#include <sched.h>
#include <nic.h>

#define SYSCALL_INTR_NO	0x80

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

unsigned long long do_syscall_interrupt(
	unsigned long long syscall_id, unsigned long long arg1,
	unsigned long long arg2, unsigned long long arg3)
{
	unsigned long long ret_val = 0;
	struct pixelformat *p;

	switch (syscall_id) {
	case SYSCALL_PUTC:
		putc((char)arg1);
		break;

	case SYSCALL_OPEN:
		ret_val = (unsigned long long)open((char *)arg1);
		break;

	case SYSCALL_EXEC:
		exec((struct file *)arg1);
		break;

	case SYSCALL_ENQ_TASK:
		enq_task((struct file *)arg1);
		break;

	case SYSCALL_RCV_FRAME:
		ret_val = receive_frame((void *)arg1);
		break;

	case SYSCALL_SND_FRAME:
		ret_val = send_frame((void *)arg1, arg2);
		break;

	case SYSCALL_EXEC_AP:
		ret_val = ap_enq_task((struct file *)arg1, arg2);
		break;

	case SYSCALL_GET_PNUM:
		ret_val = get_pnum();
		break;

	case SYSCALL_GET_PX:
		get_px(arg1, arg2, (struct pixelformat *)arg3);
		break;

	case SYSCALL_DRAW_PX:
		p = (struct pixelformat *)arg3;
		draw_px(arg1, arg2, p->r, p->g, p->b);
		break;

	case SYSCALL_SET_BG:
		set_bg(arg1, arg2, arg3);
		break;

	case SYSCALL_CLS:
		clear_screen();
		break;
	}

	/* PICへ割り込み処理終了を通知(EOI) */
	set_pic_eoi(SYSCALL_INTR_NO);

	return ret_val;
}

void syscall_handler(void);
void syscall_init(void)
{
	set_intr_desc(SYSCALL_INTR_NO, syscall_handler);
	enable_pic_intr(SYSCALL_INTR_NO);
}
