#pragma once
#include <stdint.h>

typedef struct _PADTYPE
{
	unsigned char	stat;
	unsigned char	len : 4;
	unsigned char	type : 4;
	unsigned short	btn;
	unsigned char	rs_x, rs_y;
	unsigned char	ls_x, ls_y;
} PADTYPE;