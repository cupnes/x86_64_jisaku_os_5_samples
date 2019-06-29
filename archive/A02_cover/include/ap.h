#pragma once

#include <fs.h>

void ap_init(void);
void ap_run(unsigned char pnum);
int ap_enq_task(struct file *f, unsigned char pnum);
