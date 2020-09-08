#include "tLib.h"

void tBitmapInit(tBitmap * Bitmap)
{
	Bitmap->bitmap =0;
}

void tBitmapSet(tBitmap * Bitmap, uint32_t pos)
{
	Bitmap->bitmap |= (1 << pos);
}

void tBitmapClear(tBitmap * Bitmap, uint32_t pos)
{
	Bitmap->bitmap &= ~(1 << pos);
}

uint32_t tBitmapGetFirstSet(tBitmap * Bitmap, uint8_t pos)
{
	static const uint8_t quickFindTable[]=
	{
		/* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	};
	
	if(Bitmap->bitmap & 0xFF)
	{
		return quickFindTable[Bitmap->bitmap & 0xFF];
	}
	else if(Bitmap->bitmap & 0xFF00)
	{
		return quickFindTable[(Bitmap->bitmap >> 8) & 0xFF] + 8;
	}
	else if(Bitmap->bitmap & 0xFF0000)
	{
		return quickFindTable[(Bitmap->bitmap >> 16) & 0xFF] + 16;
	}
	else if(Bitmap->bitmap & 0xFF000000)
	{
		return quickFindTable[(Bitmap->bitmap >> 24) & 0xFF] + 24;
	}
	else
	{
		return tBitmapPosCount();
	}
}

uint32_t tBitmapPosCount(void)
{
	return 32;
}
