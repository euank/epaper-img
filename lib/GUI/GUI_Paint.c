/******************************************************************************
 * | File      	:   GUI_Paint.c
 * | Author      :   Waveshare electronics
 * | Function    :	Achieve drawing: draw points, lines, boxes, circles and
 *                   their size, solid dotted line, solid rectangle hollow
 *                   rectangle, solid circle hollow circle.
 * | Info        :
 *   Achieve display characters: Display a single character, string, number
 *   Achieve time display: adaptive size display time minutes and seconds
 *----------------
 * |	This version:   V3.2
 * | Date        :   2020-07-10
 * | Info        :
 * -----------------------------------------------------------------------------
 * V3.2(2020-07-10):
 * 1.Change: Paint_SetScale(UBYTE scale)
 *		 Add scale 7 for 5.65f e-Parper
 * 2.Change: Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
 *		 Add the branch for scale 7
 * 3.Change: Paint_Clear(UWORD Color)
 *		 Add the branch for scale 7
 * -----------------------------------------------------------------------------
 * V3.1(2019-10-10):
 * 1. Add gray level
 *   PAINT Add Scale
 * 2. Add void Paint_SetScale(UBYTE scale);
 * -----------------------------------------------------------------------------
 * V3.0(2019-04-18):
 * 1.Change:
 *    Paint_DrawPoint(..., DOT_STYLE DOT_STYLE)
 * => Paint_DrawPoint(..., DOT_STYLE Dot_Style)
 *    Paint_DrawLine(..., LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawLine(..., DOT_PIXEL Line_width, LINE_STYLE Line_Style)
 *    Paint_DrawRectangle(..., DRAW_FILL Filled, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawRectangle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
 *    Paint_DrawCircle(..., DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawCircle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Filll)
 *
 * -----------------------------------------------------------------------------
 * V2.0(2018-11-15):
 * 1.add: Paint_NewImage()
 *    Create an image's properties
 * 2.add: Paint_SelectImage()
 *    Select the picture to be drawn
 * 3.add: Paint_SetRotate()
 *    Set the direction of the cache
 * 4.add: Paint_RotateImage()
 *    Can flip the picture, Support 0-360 degrees,
 *    but only 90.180.270 rotation is better
 * 4.add: Paint_SetMirroring()
 *    Can Mirroring the picture, horizontal, vertical, origin
 * 5.add: Paint_DrawString_CN()
 *    Can display Chinese(GB1312)
 *
 * -----------------------------------------------------------------------------
 * V1.0(2018-07-17):
 *   Create library
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/
#include "GUI_Paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()

PAINT Paint;

/******************************************************************************
function: Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate,
                    UWORD Color) {
  Paint.Image = NULL;
  Paint.Image = image;

  Paint.WidthMemory = Width;
  Paint.HeightMemory = Height;
  Paint.Color = Color;
  Paint.Scale = 2;
  Paint.WidthByte = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1);
  Paint.HeightByte = Height;
  //    printf("WidthByte = %d, HeightByte = %d\r\n", Paint.WidthByte,
  //    Paint.HeightByte); printf(" EPD_WIDTH / 8 = %d\r\n",  122 / 8);

  Paint.Rotate = Rotate;
  Paint.Mirror = MIRROR_NONE;

  if (Rotate == ROTATE_0 || Rotate == ROTATE_180) {
    Paint.Width = Width;
    Paint.Height = Height;
  } else {
    Paint.Width = Height;
    Paint.Height = Width;
  }
}

/******************************************************************************
function: Select Image
parameter:
    image : Pointer to the image cache
******************************************************************************/
void Paint_SelectImage(UBYTE *image) { Paint.Image = image; }

/******************************************************************************
function: Draw Pixels
parameter:
    Xpoint : At point X
    Ypoint : At point Y
    Color  : Painted colors
******************************************************************************/
void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color) {
  if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
    Debug("Exceeding display boundaries\r\n");
    return;
  }
  UWORD X, Y;
  switch (Paint.Rotate) {
  case 0:
    X = Xpoint;
    Y = Ypoint;
    break;
  case 90:
    X = Paint.WidthMemory - Ypoint - 1;
    Y = Xpoint;
    break;
  case 180:
    X = Paint.WidthMemory - Xpoint - 1;
    Y = Paint.HeightMemory - Ypoint - 1;
    break;
  case 270:
    X = Ypoint;
    Y = Paint.HeightMemory - Xpoint - 1;
    break;
  default:
    return;
  }

  switch (Paint.Mirror) {
  case MIRROR_NONE:
    break;
  case MIRROR_HORIZONTAL:
    X = Paint.WidthMemory - X - 1;
    break;
  case MIRROR_VERTICAL:
    Y = Paint.HeightMemory - Y - 1;
    break;
  case MIRROR_ORIGIN:
    X = Paint.WidthMemory - X - 1;
    Y = Paint.HeightMemory - Y - 1;
    break;
  default:
    return;
  }

  if (X > Paint.WidthMemory || Y > Paint.HeightMemory) {
    Debug("Exceeding display boundaries\r\n");
    return;
  }

  if (Paint.Scale == 2) {
    UDOUBLE Addr = X / 8 + Y * Paint.WidthByte;
    UBYTE Rdata = Paint.Image[Addr];
    if (Color == BLACK)
      Paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
    else
      Paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
  } else if (Paint.Scale == 4) {
    UDOUBLE Addr = X / 4 + Y * Paint.WidthByte;
    Color = Color % 4; // Guaranteed color scale is 4  --- 0~3
    UBYTE Rdata = Paint.Image[Addr];
    Rdata = Rdata & (~(0xC0 >> ((X % 4) * 2))); // Clear first, then set value
    Paint.Image[Addr] = Rdata | ((Color << 6) >> ((X % 4) * 2));
  } else if (Paint.Scale == 7 || Paint.Scale == 16) {
    UDOUBLE Addr = X / 2 + Y * Paint.WidthByte;
    UBYTE Rdata = Paint.Image[Addr];
    Rdata = Rdata & (~(0xF0 >> ((X % 2) * 4))); // Clear first, then set value
    Paint.Image[Addr] = Rdata | ((Color << 4) >> ((X % 2) * 4));
    // printf("Add =  %d ,data = %d\r\n",Addr,Rdata);
  }
}

/******************************************************************************
function: Clear the color of a window
parameter:
    Xstart : x starting point
    Ystart : Y starting point
    Xend   : x end point
    Yend   : y end point
    Color  : Painted colors
******************************************************************************/
void Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                        UWORD Color) {
  UWORD X, Y;
  for (Y = Ystart; Y < Yend; Y++) {
    for (X = Xstart; X < Xend; X++) { // 8 pixel =  1 byte
      Paint_SetPixel(X, Y, Color);
    }
  }
}

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    image_buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
******************************************************************************/
void Paint_DrawBitMap(const unsigned char *image_buffer) {
  UWORD x, y;
  UDOUBLE Addr = 0;

  for (y = 0; y < Paint.HeightByte; y++) {
    for (x = 0; x < Paint.WidthByte; x++) { // 8 pixel =  1 byte
      Addr = x + y * Paint.WidthByte;
      Paint.Image[Addr] = (unsigned char)image_buffer[Addr];
    }
  }
}
