#pragma once

unsigned char get_pnum(void);
unsigned long long get_icr(void);
void send_ipi(unsigned char pnum);
void apic_init(void);
