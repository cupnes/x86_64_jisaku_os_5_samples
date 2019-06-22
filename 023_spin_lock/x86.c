#include <x86.h>

/* GDTの定義 */
const unsigned long long gdt[] = {
	0x0000000000000000,	/* NULL descriptor */
	0x00af9a000000ffff,	/* base=0, limit=4GB, mode=code(r-x),kernel */
	0x00cf93000000ffff	/* base=0, limit=4GB, mode=data(rw-),kernel */
};
unsigned long long gdtr[2];

inline void enable_cpu_intr(void)
{
	asm volatile ("sti");
}

inline void cpu_halt(void)
{
	asm volatile ("hlt");
}

inline unsigned char io_read(unsigned short addr)
{
	unsigned char value;
	asm volatile ("inb %[addr], %[value]"
		      : [value]"=a"(value) : [addr]"d"(addr));
	return value;
}

inline void io_write(unsigned short addr, unsigned char value)
{
	asm volatile ("outb %[value], %[addr]"
		      :: [value]"a"(value), [addr]"d"(addr));
}

inline unsigned int io_read32(unsigned short addr)
{
	unsigned int value;
	asm volatile ("inl %[addr], %[value]"
		      : [value]"=a"(value) : [addr]"d"(addr));
	return value;
}

inline void io_write32(unsigned short addr, unsigned int value)
{
	asm volatile ("outl %[value], %[addr]"
		      :: [value]"a"(value), [addr]"d"(addr));
}

void gdt_init(void)
{
	/* GDTRの設定 */
	gdtr[0] = ((unsigned long long)gdt << 16) | (sizeof(gdt) - 1);
	gdtr[1] = ((unsigned long long)gdt >> 48);
	asm volatile ("lgdt gdtr");

	/* DS・SSの設定 */
	asm volatile ("movw $2*8, %ax\n"
		      "movw %ax, %ds\n"
		      "movw %ax, %ss\n");

	/* CSの設定 */
	unsigned short selector = SS_KERNEL_CODE;
	unsigned long long dummy;
	asm volatile ("pushq %[selector];"
		      "leaq ret_label(%%rip), %[dummy];"
		      "pushq %[dummy];"
		      "lretq;"
		      "ret_label:"
		      : [dummy]"=r"(dummy)
		      : [selector]"m"(selector));
}

/*
Spin_Lock:
	CMP lockvar, 0		;Check if lock is free
	JE Get_Lock
	PAUSE			;Short delay
	JMP Spin_Lock
Get_Lock:
	MOV EAX, 1
	XCHG EAX, lockvar	;Try to get lock
	CMP EAX, 0		;Test if successful
	JNE Spin_Lock
Critical_Section:
	<critical section code>
	MOV lockvar, 0
	...
Continue:

# ref:
# 8.10.6.1 Use the PAUSE Instruction in Spin-Wait Loops
# - Intel® 64 and IA-32 Architectures Software Developer's Manual
#   Volume 3 System Programming Guide
*/

void spin_lock(unsigned int *lockvar)
{
	unsigned char got_lock = 0;
	do {
		while (*lockvar)
			asm volatile ("pause");

		unsigned int lock = 1;
		asm volatile ("xchg %[lock], %[lockvar]"
			      : [lock]"+r"(lock), [lockvar]"+m"(*lockvar));

		if (!lock)
			got_lock = 1;
	} while (!got_lock);
}

void spin_unlock(volatile unsigned int *lockvar __attribute__((unused)))
{
	*lockvar = 0;
}
