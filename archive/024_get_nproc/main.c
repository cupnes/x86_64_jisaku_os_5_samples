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

/* コンソールの初期化が完了したか否か */
unsigned char is_con_inited = 0;

unsigned char get_pnum(void);

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
		  void *_fs_start)
{
	unsigned char pnum = get_pnum();

	/* APの場合、初期化処理をスキップ */
	if (pnum) {
		/* コンソールの初期化が完了するまで待つ */
		while (!is_con_inited);

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

	/* eax=1,ecx=0のcpuidを表示 */
	unsigned int buf[4];
	get_cpuid(1, 0, buf);
	puts("EBX ");
	puth(buf[1], 8);
	puts("\r\n");
	puts("NPROC ");
	unsigned char nproc = buf[1] >> 16;
	puth(nproc, 2);
	puts("\r\n");

	/* haltして待つ */
	while (1)
		cpu_halt();

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
