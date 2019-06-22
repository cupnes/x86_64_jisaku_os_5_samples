#include <x86.h>
#include <intr.h>
#include <pic.h>
#include <acpi.h>
#include <fb.h>
#include <kbc.h>
#include <fbcon.h>
#include <fs.h>
#include <hpet.h>
#include <sched.h>
#include <common.h>
#include <syscall.h>
#include <proc.h>
#include <pci.h>
#include <nic.h>

struct __attribute__((packed)) platform_info {
	struct framebuffer fb;
	void *rsdp;
};

#define INIT_APP	"test"
#define NUM_AP	3

struct file *ap_task[NUM_AP] = { NULL };

unsigned char get_pnum(void);

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
		  void *_fs_start)
{
	unsigned char pnum = get_pnum();

	/* APの場合、割り込み設定のみを行う */
	if (pnum) {
		/* CPU周りの初期化 */
		gdt_init();
		intr_init();

		/* システムコールの初期化 */
		syscall_init();

		/* 自分用のタスクが登録されるのを待つ */
		while (!ap_task[pnum - 1]);

		/* 実行 */
		exec(ap_task[pnum - 1]);

		/* haltして待つ */
		while (1)
			cpu_halt();
	}

	/* フレームバッファ周りの初期化 */
	fb_init(&pi->fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	/* ACPIの初期化 */
	acpi_init(pi->rsdp);

	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* 周辺ICの初期化 */
	pic_init();
	hpet_init();
	kbc_init();
	nic_init();

	/* システムコールの初期化 */
	syscall_init();

	/* ファイルシステムの初期化 */
	fs_init(_fs_start);

	/* AP1 test */
	struct file *f = open("test1");
	puts("BSP BEGIN\r\n");
	exec(f);
	puts("BSP END\r\n");
	ap_task[0] = f;

	/* haltして待つ */
	while (1)
		cpu_halt();

	/* スケジューラの初期化 */
	sched_init();

	/* CPUの割り込み有効化 */
	enable_cpu_intr();

	/* スケジューラの開始 */
	sched_start();

	/* initアプリ起動 */
	exec(open(INIT_APP));

	/* puts("\r\nBSP END\r\n"); */

	/* 各APを順番に実行 */
	/* char *task_list[] = {"test1", "test2", "test3"}; */
	/* unsigned int i; */
	/* for (i = 0; i < NUM_AP; i++) { */
	/* 	exec(open(task_list[i])); */
	/* 	/\* puth((unsigned long long)open(task_list[i]), 16); *\/ */
	/* 	puts("\r\n"); */
	/* } */

	/* /\* haltして待つ *\/ */
	/* while (1) */
	/* 	cpu_halt(); */

	/* for (i = 0; i < NUM_AP; i++) { */
		/* puts("BEGIN "); */
		/* puts(task_list[i]); */
		/* puts("\r\n"); */

		/* volatile unsigned int _wait = 70000; */
		/* while (_wait--); */

		/* タスクを登録 */
		/* ap_task[i] = open(task_list[i]); */

		/* 実行完了を待つ */
		/* while (ap_task[i]); */

		/* puts("END "); */
		/* puts(task_list[i]); */
		/* puts("\r\n"); */
	/* } */

	/* haltして待つ */
	while (1)
		cpu_halt();
}

unsigned char get_pnum(void)
{
	unsigned int pnum;

	asm volatile ("movl	0xfee00020, %[pnum]\n"
		      "shrl	$0x18, %[pnum]\n"
		      : [pnum]"=a"(pnum));

	return pnum;
}
