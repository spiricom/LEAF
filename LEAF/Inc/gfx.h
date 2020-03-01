/*
 * gfx.h
 *
 *  Created on: Jul 8, 2018
 *      Author: jeffsnyder
 */

#ifndef GFX_H_
#define GFX_H_


#include "gfx_font.h"
#include "ssd1306.h"

typedef struct _GFX
{
	int16_t WIDTH;
	int16_t HEIGHT;
	uint16_t _width;
    uint16_t _height;
    uint16_t rotation;
    uint16_t cursor_y;
	uint16_t cursor_x;
    uint16_t textsize;
    uint16_t textcolor;
    uint16_t textbgcolor;
    uint16_t wrap;
    uint16_t _cp437;
    GFXfont *gfxFont;
} GFX;

void GFXinit(GFX* myGfx, int16_t w, int16_t h); // Constructor

  // This MUST be defined by the subclass:
void GFXdrawPixel(GFX* myGfx, int16_t x, int16_t y, uint16_t color);

  // TRANSACTION API / CORE DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
void GFXstartWrite(GFX* myGfx);
void GFXwritePixel(GFX* myGfx, int16_t x, int16_t y, uint16_t color);
void GFXwriteFillRect(GFX* myGfx, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void GFXwriteFastVLine(GFX* myGfx, int16_t x, int16_t y, int16_t h, uint16_t color);
void GFXwriteFastHLine(GFX* myGfx, int16_t x, int16_t y, int16_t w, uint16_t color);
void GFXwriteLine(GFX* myGfx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void GFXendWrite(GFX* myGfx);

  // CONTROL API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
void GFXsetRotation(GFX* myGfx, uint8_t r);
void GFXinvertDisplay(GFX* myGfx, uint8_t i);

  // BASIC DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.

    // It's good to implement those, even if using transaction API
void GFXdrawFastVLine(GFX* myGfx, int16_t x, int16_t y, int16_t h, uint16_t color);
void    GFXdrawFastHLine(GFX* myGfx, int16_t x, int16_t y, int16_t w, uint16_t color);
void    GFXfillRect(GFX* myGfx, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void    GFXfillScreen(GFX* myGfx, uint16_t color);
    // Optional and probably not necessary to change
void    GFXdrawLine(GFX* myGfx, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void    GFXdrawRect(GFX* myGfx, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

  // These exist only with Adafruit_GFX (no subclass overrides)

void    GFXdrawCircle(GFX* myGfx, int16_t x0, int16_t y0, int16_t r, uint16_t color);
void    GFXdrawCircleHelper(GFX* myGfx, int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      uint16_t color);
void    GFXfillCircle(GFX* myGfx, int16_t x0, int16_t y0, int16_t r, uint16_t color);
void    GFXfillCircleHelper(GFX* myGfx, int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      int16_t delta, uint16_t color);
void    GFXdrawTriangle(GFX* myGfx, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color);
void    GFXfillTriangle(GFX* myGfx, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color);
void    GFXdrawRoundRect(GFX* myGfx, int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color);
void    GFXfillRoundRect(GFX* myGfx, int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color);
void    GFXdrawBitmap(GFX* myGfx, int16_t x, int16_t y, uint8_t *bitmap,
      int16_t w, int16_t h, uint16_t color, uint16_t bg);
void    GFXdrawChar(GFX* myGfx, int16_t x, int16_t y, unsigned char c, uint16_t color,
      uint16_t bg, uint8_t size);
void    GFXsetCursor(GFX* myGfx, int16_t x, int16_t y);
void   GFXsetTextColor(GFX* myGfx, uint16_t c, uint16_t bg);
void    GFXsetTextSize(GFX* myGfx, uint8_t s);
void   GFXsetTextWrap(GFX* myGfx, uint8_t w);
void   GFXcp437(GFX* myGfx, uint8_t x);
void    GFXsetFont(GFX* myGfx, const GFXfont *f);
void   GFXgetTextBounds(GFX* myGfx, char *string, int16_t x, int16_t y,
      int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);


void   GFXwrite(GFX* myGfx, uint8_t);


  int16_t GFXheight(GFX* myGfx );
  int16_t GFXwidth(GFX* myGfx );

  uint8_t GFXgetRotation(GFX* myGfx );

  // get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
  int16_t GFXgetCursorX(GFX* myGfx);
  int16_t GFXgetCursorY(GFX* myGfx );

  void GFXcharBounds(GFX* myGfx, char c, int16_t *x, int16_t *y,
      int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);

int OLEDparseInt(char* buffer, uint32_t myNumber, uint8_t numDigits);

int OLEDparsePitch(char* buffer, float midi);

int OLEDparsePitchClass(char* buffer, float midi);

int OLEDparseFixedFloat(char* buffer, float input, uint8_t numDigits, uint8_t numDecimal);

#endif /* GFX_H_ */
