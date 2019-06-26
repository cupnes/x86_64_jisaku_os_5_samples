#include <x86.h>
#include <intr.h>
#include <proc.h>
#include <syscall.h>
#include <fs.h>
#include <common.h>

#define MAX_APS		16

struct file *ap_task[MAX_APS] = { NULL };
unsigned int ap_task_lock[MAX_APS] = { 0 };

void ap_init(void)
{
	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* システムコールの初期化 */
	syscall_init();
}

void ap_run(unsigned char pnum)
{
	while (1) {
		/* 自分用のタスクが登録されるのを待つ */
		while (!ap_task[pnum - 1])
			CPU_PAUSE();

		/* 実行 */
		exec(ap_task[pnum - 1]);

		/* 空に戻す */
		ap_task[pnum - 1] = NULL;
	}
}

int ap_enq_task(struct file *f, unsigned char pnum)
{
	int result = -1;

	spin_lock(&ap_task_lock[pnum - 1]);

	/* 空いていればタスクを登録 */
	if (!ap_task[pnum - 1]) {
		ap_task[pnum - 1] = f;
		result = 0;
	}

	spin_unlock(&ap_task_lock[pnum - 1]);

	return result;
}
