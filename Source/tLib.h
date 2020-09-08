#ifndef TLIB_H
#define TLIB_H
#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit(tBitmap * Bitmap);
void tBitmapSet(tBitmap * Bitmap, uint32_t pos);
void tBitmapClear(tBitmap * Bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap * Bitmap, uint8_t pos);
uint32_t tBitmapPosCount(void);

#endif
