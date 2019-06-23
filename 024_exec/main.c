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
	unsigned int nproc;
};

#define INIT_APP	"test"
#define MAX_APS		16

/* コンソールの初期化が完了したか否か */
unsigned char is_con_inited = 0;

unsigned char get_pnum(void);

struct file *ap_task[MAX_APS] = { NULL };

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
	is_con_inited = 1;

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

	/* AP1で外部アプリ実行 */
	ap_task[0] = open("puta");

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

	/* haltして待つ */
	while (1)
		cpu_halt();
}

unsigned char get_pnum(void)
{
	unsigned int pnum;

	asm volatile ("movl 0xfee00020, %[pnum]\n" /* Local APIC ID Register */
		      "shrl $0x18, %[pnum]\n"
		      : [pnum]"=r"(pnum));

	return pnum;
}