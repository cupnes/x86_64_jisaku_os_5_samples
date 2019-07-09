#define LAPIC_REG_BASE	0xfee00000
#define LAPIC_ID_REG	(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x20))
#define LAPIC_ICR_1		(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x300))

unsigned char get_pnum(void)
{
	return LAPIC_ID_REG >> 24;
}
