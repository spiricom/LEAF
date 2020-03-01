/*
 * gfx.c
 *  a C port of the Adafruit GFX library
 *  Created on: Jul 8, 2018
 *      Author: jeffsnyder
 */


/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */
#include "stm32h7xx_hal.h"
#include "gfx.h"
#include "gfx_font.c"
#include "main.h"

// Many (but maybe not all) non-AVR board installs define macros
// for compatibility with existing PROGMEM-reading AVR code.
// Do our own checks and defines here for good measure...

#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...


 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

void GFXinit(GFX* myGfx, int16_t w, int16_t h)
{
	myGfx->WIDTH = w;
	myGfx->HEIGHT = h;
	myGfx->_width    = w;
	myGfx->_height   = h;
	myGfx->rotation  = 0;
	myGfx->cursor_y  = 0;
	myGfx->cursor_x = 0;
	myGfx->textsize  = 1;
	myGfx->textcolor = 0xFFFF;
	myGfx->textbgcolor = 0xFFFF;
	myGfx->wrap      = 1;
	myGfx->_cp437    = 0;
	myGfx->gfxFont   = NULL;
}

// Bresenham's algorithm - thx wikpedia
void GFXwriteLine(GFX* myGFX, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
        	GFXwritePixel(myGFX, y0, x0, color);
        } else {
        	GFXwritePixel(myGFX, x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void GFXstartWrite(GFX* myGFX){
    // Overwrite in subclasses if desired!
}

// the most basic function, set a single pixel
void GFXdrawPixel(GFX* myGFX, int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= myGFX->_width) || (y < 0) || (y >= myGFX->_height))
    return;

  // check rotation, move pixel around if necessary
  switch (GFXgetRotation(myGFX)) {
  case 1:
    _swap_int16_t(x, y);
    x = myGFX->WIDTH - x - 1;
    break;
  case 2:
    x = myGFX->WIDTH - x - 1;
    y = myGFX->HEIGHT - y - 1;
    break;
  case 3:
	_swap_int16_t(x, y);
    y = myGFX->HEIGHT - y - 1;
    break;
  }

  // x is which column
    switch (color)
    {
      case WHITE:   buffer[x+ (y/8)*myGFX->WIDTH] |=  (1 << (y&7)); break;
      case BLACK:   buffer[x+ (y/8)*myGFX->WIDTH] &= ~(1 << (y&7)); break;
      case INVERSE: buffer[x+ (y/8)*myGFX->WIDTH] ^=  (1 << (y&7)); break;
    }

}

void GFXwritePixel(GFX* myGFX,int16_t x, int16_t y, uint16_t color){
    // Overwrite in subclasses if startWrite is defined!
    GFXdrawPixel(myGFX, x, y, color);
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void GFXwriteFastVLine(GFX* myGFX,int16_t x, int16_t y,
        int16_t h, uint16_t color) {
    // Overwrite in subclasses if startWrite is defined!
    // Can be just writeLine(x, y, x, y+h-1, color);
    // or writeFillRect(x, y, 1, h, color);
    GFXdrawFastVLine(myGFX, x, y, h, color);
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void GFXwriteFastHLine(GFX* myGFX, int16_t x, int16_t y,
        int16_t w, uint16_t color) {
    // Overwrite in subclasses if startWrite is defined!
    // Example: writeLine(x, y, x+w-1, y, color);
    // or writeFillRect(x, y, w, 1, color);
    GFXdrawFastHLine(myGFX,x, y, w, color);
}

void GFXwriteFillRect(GFX* myGFX,int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    // Overwrite in subclasses if desired!
    GFXfillRect(myGFX,x,y,w,h,color);
}

void GFXendWrite(GFX* myGFX){
    // Overwrite in subclasses if startWrite is defined!
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call drawLine() instead
void GFXdrawFastVLine(GFX* myGFX,int16_t x, int16_t y,
        int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    GFXwriteLine(myGFX,x, y, x, y+h-1, color);
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call drawLine() instead
void GFXdrawFastHLine(GFX* myGFX,int16_t x, int16_t y,
        int16_t w, uint16_t color) {
    // Update in subclasses if desired!
    GFXwriteLine(myGFX,x, y, x+w-1, y, color);
}

void GFXfillRect(GFX* myGFX,int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    // Update in subclasses if desired!
    for (int16_t i=x; i<x+w; i++) {
        GFXwriteFastVLine(myGFX,i, y, h, color);
    }
}

void GFXfillScreen(GFX* myGFX,uint16_t color) {
    // Update in subclasses if desired!
    GFXfillRect(myGFX,0, 0, myGFX->_width, myGFX->_height, color);
}

void GFXdrawLine(GFX* myGFX,int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color) {
    // Update in subclasses if desired!
    if(x0 == x1){
        if(y0 > y1) _swap_int16_t(y0, y1);
        GFXdrawFastVLine(myGFX,x0, y0, y1 - y0 + 1, color);
    } else if(y0 == y1){
        if(x0 > x1) _swap_int16_t(x0, x1);
        GFXdrawFastHLine(myGFX,x0, y0, x1 - x0 + 1, color);
    } else {
        GFXwriteLine(myGFX,x0, y0, x1, y1, color);
    }
}

// Draw a circle outline
void GFXdrawCircle(GFX* myGFX,int16_t x0, int16_t y0, int16_t r,
        uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    GFXwritePixel(myGFX,x0  , y0+r, color);
    GFXwritePixel(myGFX,x0  , y0-r, color);
    GFXwritePixel(myGFX,x0+r, y0  , color);
    GFXwritePixel(myGFX,x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        GFXwritePixel(myGFX,x0 + x, y0 + y, color);
        GFXwritePixel(myGFX,x0 - x, y0 + y, color);
        GFXwritePixel(myGFX,x0 + x, y0 - y, color);
        GFXwritePixel(myGFX,x0 - x, y0 - y, color);
        GFXwritePixel(myGFX,x0 + y, y0 + x, color);
        GFXwritePixel(myGFX,x0 - y, y0 + x, color);
        GFXwritePixel(myGFX,x0 + y, y0 - x, color);
        GFXwritePixel(myGFX,x0 - y, y0 - x, color);
    }

}

void GFXdrawCircleHelper(GFX* myGFX,int16_t x0, int16_t y0,
        int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            GFXwritePixel(myGFX, x0 + x, y0 + y, color);
            GFXwritePixel(myGFX, x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
        	GFXwritePixel(myGFX, x0 + x, y0 - y, color);
        	GFXwritePixel(myGFX, x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
        	GFXwritePixel(myGFX, x0 - y, y0 + x, color);
        	GFXwritePixel(myGFX, x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
        	GFXwritePixel(myGFX, x0 - y, y0 - x, color);
        	GFXwritePixel(myGFX, x0 - x, y0 - y, color);
        }
    }
}

void GFXfillCircle(GFX* myGFX, int16_t x0, int16_t y0, int16_t r,
        uint16_t color) {

    GFXwriteFastVLine(myGFX, x0, y0-r, 2*r+1, color);
    GFXfillCircleHelper(myGFX, x0, y0, r, 3, 0, color);

}

// Used to do circles and roundrects
void GFXfillCircleHelper(GFX* myGFX, int16_t x0, int16_t y0, int16_t r,
        uint8_t cornername, int16_t delta, uint16_t color) {

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) {
            GFXwriteFastVLine(myGFX, x0+x, y0-y, 2*y+1+delta, color);
            GFXwriteFastVLine(myGFX, x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
        	GFXwriteFastVLine(myGFX, x0-x, y0-y, 2*y+1+delta, color);
        	GFXwriteFastVLine(myGFX, x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

// Draw a rectangle
void GFXdrawRect(GFX* myGFX, int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {

    GFXwriteFastHLine(myGFX, x, y, w, color);
    GFXwriteFastHLine(myGFX, x, y+h-1, w, color);
    GFXwriteFastVLine(myGFX, x, y, h, color);
    GFXwriteFastVLine(myGFX, x+w-1, y, h, color);

}

// Draw a rounded rectangle
void GFXdrawRoundRect(GFX* myGFX, int16_t x, int16_t y, int16_t w,
        int16_t h, int16_t r, uint16_t color) {
    // smarter version
    GFXwriteFastHLine(myGFX, x+r  , y    , w-2*r, color); // Top
    GFXwriteFastHLine(myGFX, x+r  , y+h-1, w-2*r, color); // Bottom
    GFXwriteFastVLine(myGFX, x    , y+r  , h-2*r, color); // Left
    GFXwriteFastVLine(myGFX, x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    GFXdrawCircleHelper(myGFX, x+r    , y+r    , r, 1, color);
    GFXdrawCircleHelper(myGFX, x+w-r-1, y+r    , r, 2, color);
    GFXdrawCircleHelper(myGFX, x+w-r-1, y+h-r-1, r, 4, color);
    GFXdrawCircleHelper(myGFX, x+r    , y+h-r-1, r, 8, color);

}

// Fill a rounded rectangle
void GFXfillRoundRect(GFX* myGFX, int16_t x, int16_t y, int16_t w,
        int16_t h, int16_t r, uint16_t color) {
    // smarter version
    GFXwriteFillRect(myGFX, x+r, y, w-2*r, h, color);
    // draw four corners
    GFXfillCircleHelper(myGFX, x+w-r-1, y+r, r, 1, h-2*r-1, color);
    GFXfillCircleHelper(myGFX, x+r    , y+r, r, 2, h-2*r-1, color);

}

// Draw a triangle
void GFXdrawTriangle(GFX* myGFX, int16_t x0, int16_t y0,
        int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    GFXdrawLine(myGFX, x0, y0, x1, y1, color);
    GFXdrawLine(myGFX, x1, y1, x2, y2, color);
    GFXdrawLine(myGFX, x2, y2, x0, y0, color);
}

// Fill a triangle
void GFXfillTriangle(GFX* myGFX, int16_t x0, int16_t y0,
        int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }


    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        GFXwriteFastHLine(myGFX, a, y0, b-a+1, color);

        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        GFXwriteFastHLine(myGFX,a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        GFXwriteFastHLine(myGFX, a, y, b-a+1, color);
    }

}

// BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------



// Draw a RAM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground (for set bits) and background (unset
// bits) colors.
void GFXdrawBitmap(GFX* myGFX, int16_t x, int16_t y,
  uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    startWrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            if(i & 7) byte <<= 1;
            else      byte   = bitmap[j * byteWidth + i / 8];
            GFXwritePixel(myGFX, x+i, y, (byte & 0x80) ? color : bg);
        }
    }
    endWrite();
}



// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
void GFXdrawChar(GFX* myGFX, int16_t x, int16_t y, unsigned char c,
  uint16_t color, uint16_t bg, uint8_t size) {

    if(!myGFX->gfxFont) { // 'Classic' built-in font

        if((x >= myGFX->_width)            || // Clip right
           (y >= myGFX->_height)           || // Clip bottom
           ((x + 6 * size - 1) < 0) || // Clip left
           ((y + 8 * size - 1) < 0))   // Clip top
            return;

        if(!myGFX->_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior


        for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
            uint8_t line = pgm_read_byte(&standardGFXfont[c * 5 + i]);
            for(int8_t j=0; j<8; j++, line >>= 1) {
                if(line & 1) {
                    if(size == 1)
                        GFXwritePixel(myGFX, x+i, y+j, color);
                    else
                        GFXwriteFillRect(myGFX, x+i*size, y+j*size, size, size, color);
                } else if(bg != color) {
                    if(size == 1)
                        GFXwritePixel(myGFX, x+i, y+j, bg);
                    else
                        GFXwriteFillRect(myGFX, x+i*size, y+j*size, size, size, bg);
                }
            }
        }
        if(bg != color) { // If opaque, draw vertical line for last column
            if(size == 1) GFXwriteFastVLine(myGFX, x+5, y, 8, bg);
            else          GFXwriteFillRect(myGFX, x+5*size, y, size, 8*size, bg);
        }


    } else { // Custom font

        // Character is assumed previously filtered by write() to eliminate
        // newlines, returns, non-printable characters, etc.  Calling
        // drawChar() directly with 'bad' characters of font may cause mayhem!

        c -= (uint8_t)pgm_read_byte(&myGFX->gfxFont->first);
        GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&myGFX->gfxFont->glyph))[c]);
        uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&myGFX->gfxFont->bitmap);

        uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
        uint8_t  w  = pgm_read_byte(&glyph->width),
                 h  = pgm_read_byte(&glyph->height);
        int8_t   xo = pgm_read_byte(&glyph->xOffset),
                 yo = pgm_read_byte(&glyph->yOffset);
        uint8_t  xx, yy, bits = 0, bit = 0;
        int16_t  xo16 = 0, yo16 = 0;

        if(size > 1) {
            xo16 = xo;
            yo16 = yo;
        }

        // Todo: Add character clipping here

        // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
        // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
        // has typically been used with the 'classic' font to overwrite old
        // screen contents with new data.  This ONLY works because the
        // characters are a uniform size; it's not a sensible thing to do with
        // proportionally-spaced fonts with glyphs of varying sizes (and that
        // may overlap).  To replace previously-drawn text when using a custom
        // font, use the getTextBounds() function to determine the smallest
        // rectangle encompassing a string, erase the area with fillRect(),
        // then draw new text.  This WILL infortunately 'blink' the text, but
        // is unavoidable.  Drawing 'background' pixels will NOT fix this,
        // only creates a new set of problems.  Have an idea to work around
        // this (a canvas object type for MCUs that can afford the RAM and
        // displays supporting setAddrWindow() and pushColors()), but haven't
        // implemented this yet.


        for(yy=0; yy<h; yy++) {
            for(xx=0; xx<w; xx++) {
                if(!(bit++ & 7)) {
                    bits = pgm_read_byte(&bitmap[bo++]);
                }
                if(bits & 0x80) {
                    if(size == 1) {
                        GFXwritePixel(myGFX, x+xo+xx, y+yo+yy, color);
                    } else {
                        GFXwriteFillRect(myGFX, x+(xo16+xx)*size, y+(yo16+yy)*size,
                          size, size, color);
                    }
                }
                bits <<= 1;
            }
        }


    } // End classic vs custom font
}


void GFXwrite(GFX* myGFX, uint8_t c) {

    if(!myGFX->gfxFont) { // 'Classic' built-in font

        if(c == '\n') {                        // Newline?
        	myGFX->cursor_x  = 0;                     // Reset x to zero,
        	myGFX->cursor_y += myGFX->textsize * 8;          // advance y one line
        } else if(c != '\r') {                 // Ignore carriage returns
            if(myGFX->wrap && ((myGFX->cursor_x + myGFX->textsize * 6) > myGFX->_width)) { // Off right?
            	myGFX->cursor_x  = 0;                 // Reset x to zero,
            	myGFX->cursor_y += myGFX->textsize * 8;      // advance y one line
            }
            GFXdrawChar(myGFX, myGFX->cursor_x, myGFX->cursor_y, c, myGFX->textcolor, myGFX->textbgcolor, myGFX->textsize);
            myGFX->cursor_x += myGFX->textsize * 6;          // Advance x one char
        }

    } else { // Custom font

        if(c == '\n') {
        	myGFX->cursor_x  = 0;
        	myGFX->cursor_y += (int16_t)myGFX->textsize *
                        (uint8_t)pgm_read_byte(&myGFX->gfxFont->yAdvance);
        } else if(c != '\r') {
            uint8_t first = pgm_read_byte(&myGFX->gfxFont->first);
            if((c >= first) && (c <= (uint8_t)pgm_read_byte(&myGFX->gfxFont->last))) {
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(
                  &myGFX->gfxFont->glyph))[c - first]);
                uint8_t   w     = pgm_read_byte(&glyph->width),
                          h     = pgm_read_byte(&glyph->height);
                if((w > 0) && (h > 0)) { // Is there an associated bitmap?
                    int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
                    if(myGFX->wrap && ((myGFX->cursor_x + myGFX->textsize * (xo + w)) > myGFX->_width)) {
                    	myGFX->cursor_x  = 0;
                    	myGFX->cursor_y += (int16_t)myGFX->textsize *
                          (uint8_t)pgm_read_byte(&myGFX->gfxFont->yAdvance);
                    }
                    GFXdrawChar(myGFX, myGFX->cursor_x, myGFX->cursor_y, c, myGFX->textcolor, myGFX->textbgcolor, myGFX->textsize);
                }
                myGFX->cursor_x += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)myGFX->textsize;
            }
        }

    }

}

void GFXsetCursor(GFX* myGFX, int16_t x, int16_t y) {
	myGFX->cursor_x = x;
	myGFX->cursor_y = y;
}

int16_t GFXgetCursorX(GFX* myGFX)
{
    return myGFX->cursor_x;
}

int16_t GFXgetCursorY(GFX* myGFX)
{
    return myGFX->cursor_y;
}

void GFXsetTextSize(GFX* myGFX, uint8_t s) {
	myGFX->textsize = (s > 0) ? s : 1;
}

void GFXsetTextColor(GFX* myGFX, uint16_t c, uint16_t b) {
	myGFX->textcolor   = c;
	myGFX->textbgcolor = b;
}

void GFXsetTextWrap(GFX* myGFX, uint8_t w) {
	myGFX->wrap = w;
}

uint8_t GFXgetRotation(GFX* myGFX)  {
    return myGFX->rotation;
}

void GFXsetRotation(GFX* myGFX, uint8_t x) {
	myGFX->rotation = (x & 3);
    switch(myGFX->rotation) {
        case 0:
        case 2:
        	myGFX->_width  = myGFX->WIDTH;
        	myGFX->_height = myGFX->HEIGHT;
            break;
        case 1:
        case 3:
        	myGFX->_width  = myGFX->HEIGHT;
        	myGFX->_height = myGFX->WIDTH;
            break;
    }
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void GFXcp437(GFX* myGFX, uint8_t x) {
	myGFX->_cp437 = x;
}

void GFXsetFont(GFX* myGFX, const GFXfont *f) {
    if(f) {            // Font struct pointer passed in?
        if(!myGFX->gfxFont) { // And no current font struct?
            // Switching from classic to new font behavior.
            // Move cursor pos down 6 pixels so it's on baseline.
        	myGFX->cursor_y += 6;
        }
    } else if(myGFX->gfxFont) { // NULL passed.  Current font struct defined?
        // Switching from new to classic font behavior.
        // Move cursor pos up 6 pixels so it's at top-left of char.
    	myGFX->cursor_y -= 6;
    }
    myGFX->gfxFont = (GFXfont *)f;
}

// Broke this out as it's used by both the PROGMEM- and RAM-resident
// getTextBounds() functions.
void GFXcharBounds(GFX* myGFX, char c, int16_t *x, int16_t *y,
  int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {

    if(myGFX->gfxFont) {

        if(c == '\n') { // Newline?
            *x  = 0;    // Reset x to zero, advance y by one line
            *y += myGFX->textsize * (uint8_t)pgm_read_byte(&myGFX->gfxFont->yAdvance);
        } else if(c != '\r') { // Not a carriage return; is normal char
            uint8_t first = pgm_read_byte(&myGFX->gfxFont->first),
                    last  = pgm_read_byte(&myGFX->gfxFont->last);
            if((c >= first) && (c <= last)) { // Char present in this font?
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(
                  &myGFX->gfxFont->glyph))[c - first]);
                uint8_t gw = pgm_read_byte(&glyph->width),
                        gh = pgm_read_byte(&glyph->height),
                        xa = pgm_read_byte(&glyph->xAdvance);
                int8_t  xo = pgm_read_byte(&glyph->xOffset),
                        yo = pgm_read_byte(&glyph->yOffset);
                if(myGFX->wrap && ((*x+(((int16_t)xo+gw)*myGFX->textsize)) > myGFX->_width)) {
                    *x  = 0; // Reset x to zero, advance y by one line
                    *y += myGFX->textsize * (uint8_t)pgm_read_byte(&myGFX->gfxFont->yAdvance);
                }
                int16_t ts = (int16_t)myGFX->textsize,
                        x1 = *x + xo * ts,
                        y1 = *y + yo * ts,
                        x2 = x1 + gw * ts - 1,
                        y2 = y1 + gh * ts - 1;
                if(x1 < *minx) *minx = x1;
                if(y1 < *miny) *miny = y1;
                if(x2 > *maxx) *maxx = x2;
                if(y2 > *maxy) *maxy = y2;
                *x += xa * ts;
            }
        }

    } else { // Default font

        if(c == '\n') {                     // Newline?
            *x  = 0;                        // Reset x to zero,
            *y += myGFX->textsize * 8;             // advance y one line
            // min/max x/y unchaged -- that waits for next 'normal' character
        } else if(c != '\r') {  // Normal char; ignore carriage returns
            if(myGFX->wrap && ((*x + myGFX->textsize * 6) > myGFX->_width)) { // Off right?
                *x  = 0;                    // Reset x to zero,
                *y += myGFX->textsize * 8;         // advance y one line
            }
            int x2 = *x + myGFX->textsize * 6 - 1, // Lower-right pixel of char
                y2 = *y + myGFX->textsize * 8 - 1;
            if(x2 > *maxx) *maxx = x2;      // Track max x, y
            if(y2 > *maxy) *maxy = y2;
            if(*x < *minx) *minx = *x;      // Track min x, y
            if(*y < *miny) *miny = *y;
            *x += myGFX->textsize * 6;             // Advance x one char
        }
    }
}

// Pass string and a cursor position, returns UL corner and W,H.
void GFXgetTextBounds(GFX* myGFX, char *str, int16_t x, int16_t y,
        int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t c; // Current character

    *x1 = x;
    *y1 = y;
    *w  = *h = 0;

    int16_t minx = myGFX->_width, miny = myGFX->_height, maxx = -1, maxy = -1;

    while((c = *str++))
    	GFXcharBounds(myGFX, c, &x, &y, &minx, &miny, &maxx, &maxy);

    if(maxx >= minx) {
        *x1 = minx;
        *w  = maxx - minx + 1;
    }
    if(maxy >= miny) {
        *y1 = miny;
        *h  = maxy - miny + 1;
    }
}



// Return the size of the display (per current rotation)
int16_t GFXwidth(GFX* myGFX ) {
    return myGFX->_width;
}

int16_t GFXheight(GFX* myGFX)  {
    return myGFX->_height;
}

void GFXinvertDisplay(GFX* myGFX, uint8_t i) {
    // Do nothing, must be subclassed if supported by hardware
}

uint8_t pitches[24] =
{
	'C', ' ',
	'C', '#',
	'D', ' ',
	'D', '#',
	'E', ' ',
	'F', ' ',
	'F', '#',
	'G', ' ',
	'G', '#',
	'A', ' ',
	'A', '#',
	'B', ' '
};

int OLEDparseInt(char* buffer, uint32_t myNumber, uint8_t numDigits)
{
	for (int i = 0; i < numDigits; i++)
	{
		int whichPlace = (uint32_t)(powf(10.0f,(numDigits - 1) - i));
		int thisDigit = (myNumber / whichPlace);
		buffer[i] = thisDigit + 48;
		myNumber -= thisDigit * whichPlace;
	}

	return numDigits;
}

int OLEDparsePitch(char* buffer, float midi)
{
	int pclass, octave, note, neg = 0; float offset;

	note = (int)midi;
	offset = midi - note;

	if ((midi + 0.5f) > (note+1))
	{
		note += 1;
		offset = (1.0f - offset) + 0.01f;
		neg = 1;
	}

	pclass = (note % 12);
	octave = (int)(note / 12) - 1;

	int idx = 0;

	buffer[idx++] = pitches[pclass*2];
	buffer[idx++] = pitches[pclass*2+1];

	OLEDparseInt(&buffer[idx++], octave, 1);

	buffer[idx++] = ' ';

	if (neg == 1)
		buffer[idx++] = '-';
	else
		buffer[idx++] = '+';

	OLEDparseInt(&buffer[idx], (uint32_t) (offset * 100.0f), 2);

	return idx+2;
}

int OLEDparsePitchClass(char* buffer, float midi)
{
	int pclass, note;
	float offset;

	note = (int)midi;
	offset = midi - note;

	if ((midi + 0.5f) > (note+1))
	{
		note += 1;
		offset = (1.0f - offset) + 0.01f;
	}

	pclass = (note % 12);

	int idx = 0;

	buffer[idx++] = pitches[pclass*2];
	buffer[idx++] = pitches[pclass*2+1];

	return idx;
}

int OLEDparseFixedFloat(char* buffer, float input, uint8_t numDigits, uint8_t numDecimal)
{
	float power = powf(10.0f, numDecimal);
	float f = ((float)(int)(input * power + 0.5f)) / power;

	int nonzeroHasHappened = 0, decimalHasHappened = 0;

	int myNumber = (int)(f * power);

	int idx = 0, i = 0;

	if (myNumber < 0)
	{
		myNumber *= -1;
		buffer[idx++] = '-';
		i++;
	}

	while (i < numDigits)
	{
		if ((decimalHasHappened == 0) && ((numDigits-i) == numDecimal))
		{
			if (nonzeroHasHappened == 0)
			{
				idx++; //get past the negative if there is one - this is hacky should figure out how to do it right
				buffer[idx-1] = '0';
				nonzeroHasHappened = 1;
			}

			buffer[idx++] = '.';
			decimalHasHappened = 1;
		}
		else
		{

			int whichPlace = (uint32_t) powf(10.0f,(numDigits - 1 - i));
			int thisDigit = (myNumber / whichPlace);

			if (nonzeroHasHappened == 0)
			{
				if (thisDigit > 0)
				{
					buffer[idx++] = thisDigit + 48;
					nonzeroHasHappened = 1;
				}
				else
				{
					buffer[idx++] = ' ';
				}
			}
			else
			{
				buffer[idx++] = thisDigit + 48;
			}

			myNumber -= thisDigit * whichPlace;

			i++;
		}
	}

	return idx;
}
