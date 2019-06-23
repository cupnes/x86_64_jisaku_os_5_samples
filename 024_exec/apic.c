#define LAPIC_REG_BASE	0xfee00000
#define LAPIC_ID_REG	(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x20))

unsigned char get_pnum(void)
{
	return LAPIC_ID_REG >> 24;
}
