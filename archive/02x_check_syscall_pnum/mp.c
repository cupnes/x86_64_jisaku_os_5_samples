unsigned char get_pnum(void)
{
	unsigned int pnum;

	asm volatile ("movl	0xfee00020, %[pnum]\n"
		      "shrl	$0x18, %[pnum]\n"
		      : [pnum]"=a"(pnum));

	return pnum;
}
