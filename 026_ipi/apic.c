#define LAPIC_REG_BASE	0xfee00000
#define LAPIC_ID_REG	(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x20))
#define LAPIC_ICR_00_31	(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x300))
#define LAPIC_ICR_32_63	(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x310))

unsigned char get_pnum(void)
{
	return LAPIC_ID_REG >> 24;
}

unsigned long long get_icr(void)
{
	return ((unsigned long long)LAPIC_ICR_32_63 << 32) | LAPIC_ICR_00_31;
}
