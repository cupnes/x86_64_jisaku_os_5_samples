#include <pic.h>
#include <intr.h>

#define LAPIC_REG_BASE	0xfee00000
#define LAPIC_ID_REG	(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x20))
#define LAPIC_ICR_00_31	(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x300))
#define LAPIC_ICR_32_63	(*(volatile unsigned int *)(LAPIC_REG_BASE + 0x310))
#define LAPIC_ICR_DST_FLD_SHIFT	24
#define LAPIC_ICR_LEVEL_ASSERT	(1U << 14)
#define IPI_INTR_NO	0x70
#define LAPIC_ICR_VECTOR	IPI_INTR_NO

void ipi_handler(void);

unsigned char get_pnum(void)
{
	return LAPIC_ID_REG >> 24;
}

unsigned long long get_icr(void)
{
	return ((unsigned long long)LAPIC_ICR_32_63 << 32) | LAPIC_ICR_00_31;
}

void send_ipi(unsigned char pnum)
{
	LAPIC_ICR_32_63 = pnum << LAPIC_ICR_DST_FLD_SHIFT;
	LAPIC_ICR_00_31 = LAPIC_ICR_LEVEL_ASSERT | LAPIC_ICR_VECTOR;
}

void apic_init(void)
{
	set_intr_desc(IPI_INTR_NO, ipi_handler);
	enable_pic_intr(IPI_INTR_NO);
}
