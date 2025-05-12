#ifndef AUDIO_BITMAP_H
#define AUDIO_BITMAP_H

#include "AudioGlobal.h"

#define MAX_PRIORITY_COUNTS 256

typedef struct __Bitmp_t 
{
	uint32_t bitMap[8];
}Bitmap_t;

void BitmapInit (Bitmap_t* pBitmap);
int BitmapPositionCount (void);
void BitmapSet (Bitmap_t * pBitmap, int pos);
void BitmapClear (Bitmap_t * pBitmap, int pos);
int GetFirstBitSetPos (Bitmap_t * pBitmap);

#endif

#include "AudioBitmap.h"

void BitmapInit (Bitmap_t* pBitmap)
{
    for (int i = 0; i < 8; ++i)
    {
        pBitmap->bitMap[i] = 0;
    }	
}

int BitmapPositionCount (void)
{
	return MAX_PRIORITY_COUNTS;
}

void BitmapSet (Bitmap_t * pBitmap, int pos)
{
    if (pos >= MAX_PRIORITY_COUNTS)
    {
        return;
    }
    uint32_t wordIndex = pos / 32;
    uint32_t bitIndex = pos % 32;
    pBitmap->bitMap[wordIndex] |= 1 << bitIndex;
}

void BitmapClear (Bitmap_t * pBitmap, int pos)
{
    if (pos >= MAX_PRIORITY_COUNTS)
    {
        return;
    }
    uint32_t wordIndex = pos / 32;
    uint32_t bitIndex = pos % 32;
    pBitmap->bitMap[wordIndex] &= ~(1 << bitIndex);
}

int GetFirstBitSetPos(Bitmap_t* pBitmap) {
    for (int i = 0; i < 8; ++i) {
        if (pBitmap->bitMap[i] != 0) {
            static const uint8_t quickFindTable[] = {
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

            if (pBitmap->bitMap[i] & 0xFF) {
                return (i * 32) + quickFindTable[pBitmap->bitMap[i] & 0xFF];
            } else if (pBitmap->bitMap[i] & 0xFF00) {
                return (i * 32) + quickFindTable[(pBitmap->bitMap[i] >> 8) & 0xFF] + 8;
            } else if (pBitmap->bitMap[i] & 0xFF0000) {
                return (i * 32) + quickFindTable[(pBitmap->bitMap[i] >> 16) & 0xFF] + 16;
            } else if (pBitmap->bitMap[i] & 0xFF000000) {
                return (i * 32) + quickFindTable[(pBitmap->bitMap[i] >> 24) & 0xFF] + 24;
            }
        } 
    }
    return BitmapPositionCount();
}
