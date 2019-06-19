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
	void *fs_start;
	unsigned int nproc;
};

#define INIT_APP	"test"

/* プロセッサ番号(pnum)順に各プロセッサが画面出力する為に使用 */
unsigned char pnum_order = 0;

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
		  unsigned long long pnum)
{
	/* APの場合、初期化処理をスキップ */
	if (pnum) {
		/* 自分が話せる番を待つ */
		while (pnum_order != pnum);

		/* プロセッサ番号を表示 */
		puth(pnum, 1);
		pnum_order++;

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

	/* プロセッサ番号を表示 */
	puth(pnum, 1);
	pnum_order++;

	/* haltして待つ */
	while (1)
		cpu_halt();

	/* 周辺ICの初期化 */
	pic_init();
	hpet_init();
	kbc_init();
	nic_init();

	/* システムコールの初期化 */
	syscall_init();

	/* ファイルシステムの初期化 */
	fs_init(pi->fs_start);

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