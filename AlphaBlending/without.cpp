
#include "TXLib.h"
#include <emmintrin.h>

//-------------------------------------------------------------------------------------------------

typedef RGBQUAD (&scr_t) [600][800];

inline scr_t LoadImage (const char* filename)
    {
    RGBQUAD* mem = NULL;
    HDC dc = txCreateDIBSection (800, 600, &mem);
    txBitBlt (dc, 0, 0, 0, 0, dc, 0, 0, BLACKNESS);

    HDC image = txLoadImage (filename);
    txBitBlt (dc, (txGetExtentX (dc) - txGetExtentX (image)) / 2, 
                  (txGetExtentY (dc) - txGetExtentY (image)) / 2, 0, 0, image);
    txDeleteDC (image);

    return (scr_t) *mem;
    }

//-------------------------------------------------------------------------------------------------

const char I = 255u, Z = 0x80u;
const __m128i   _0 =                    _mm_set_epi8 (0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
const __m128i _255 = _mm_cvtepu8_epi16 (_mm_set_epi8 (I,I,I,I, I,I,I,I, I,I,I,I, I,I,I,I));

int main()
    {
    txCreateWindow (800, 600);
    Win32::_fpreset();
    txBegin();

    scr_t front = (scr_t) LoadImage ("AskhatCat.bmp");
    scr_t back  = (scr_t) LoadImage ("Table.bmp");
    scr_t scr   = (scr_t) *txVideoMemory();

    for (int n = 0; n <= 1000; n++)
        {
        if (GetAsyncKeyState (VK_ESCAPE)) break;
        
        if (!GetKeyState (VK_SCROLL))
            {
            for (int y = 0; y < 600; y++) 
            for (int x = 0; x < 800; x++)
                {
                RGBQUAD* fr = &front[y][x];
                RGBQUAD* bk = &back [y][x];
                
                uint16_t a  = fr->rgbReserved;

                scr[y][x]   = { (BYTE) ( (fr->rgbBlue  * (a) + bk->rgbBlue  * (255-a)) >> 8 ),
                                (BYTE) ( (fr->rgbGreen * (a) + bk->rgbGreen * (255-a)) >> 8 ),
                                (BYTE) ( (fr->rgbRed   * (a) + bk->rgbRed   * (255-a)) >> 8 ) };
                }
            }
                
        printf ("\t\r%2.0lf", txGetFPS<50>() * 10);
        txUpdateWindow();
        }

    txDisableAutoPause();
    }
    


