/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-07-31
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DEV_Config.h"

int GPIO_Handle;
int SPI_Handle;

/**
 * GPIO
 **/
int EPD_RST_PIN;
int EPD_DC_PIN;
int EPD_CS_PIN;
int EPD_BUSY_PIN;
int EPD_PWR_PIN;
int EPD_MOSI_PIN;
int EPD_SCLK_PIN;

/**
 * GPIO read and write
 **/
void DEV_Digital_Write(UWORD Pin, UBYTE Value) {
  lgGpioWrite(GPIO_Handle, Pin, Value);
}

UBYTE DEV_Digital_Read(UWORD Pin) {
  UBYTE Read_value = 0;
  Read_value = lgGpioRead(GPIO_Handle, Pin);
  return Read_value;
}

/**
 * SPI
 **/
void DEV_SPI_WriteByte(uint8_t Value) {
  lgSpiWrite(SPI_Handle, (char *)&Value, 1);
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len) {
  lgSpiWrite(SPI_Handle, (char *)pData, Len);
}

/**
 * GPIO Mode
 **/
void DEV_GPIO_Mode(UWORD Pin, UWORD Mode) {
  if (Mode == 0 || Mode == LG_SET_INPUT) {
    lgGpioClaimInput(GPIO_Handle, LFLAGS, Pin);
  } else {
    lgGpioClaimOutput(GPIO_Handle, LFLAGS, Pin, LG_LOW);
  }
}

/**
 * delay x ms
 **/
void DEV_Delay_ms(UDOUBLE xms) { lguSleep(xms / 1000.0); }

void DEV_GPIO_Init(void) {
  EPD_PWR_PIN = 14;
  EPD_MOSI_PIN = 20;
  EPD_SCLK_PIN = 21;
#if 0
	SPI_Handle = lgSpiOpen(1, 0, 10000000, 0);
	EPD_RST_PIN     = 5;
	EPD_DC_PIN      = 6;
	EPD_CS_PIN      = /* 18; */ 3;
	EPD_BUSY_PIN    = 26;
#else
  SPI_Handle = lgSpiOpen(1, 0, 10000000, 0);
  // screen 2
  EPD_RST_PIN = 0;
  EPD_DC_PIN = 12;
  EPD_CS_PIN = 4;
  EPD_BUSY_PIN = 13;
#endif

  DEV_GPIO_Mode(EPD_BUSY_PIN, 0);
  DEV_GPIO_Mode(EPD_RST_PIN, 1);
  DEV_GPIO_Mode(EPD_DC_PIN, 1);
  DEV_GPIO_Mode(EPD_CS_PIN, 1);
  DEV_GPIO_Mode(EPD_PWR_PIN, 1);
  // DEV_GPIO_Mode(EPD_MOSI_PIN, 0);
  // DEV_GPIO_Mode(EPD_SCLK_PIN, 1);

  DEV_Digital_Write(EPD_CS_PIN, 1);
  DEV_Digital_Write(EPD_PWR_PIN, 1);
}

void DEV_SPI_SendnData(UBYTE *Reg) {
  UDOUBLE size;
  size = sizeof(Reg);
  for (UDOUBLE i = 0; i < size; i++) {
    DEV_SPI_SendData(Reg[i]);
  }
}

void DEV_SPI_SendData(UBYTE Reg) {
  UBYTE i, j = Reg;
  DEV_GPIO_Mode(EPD_MOSI_PIN, 1);
  DEV_Digital_Write(EPD_CS_PIN, 0);
  for (i = 0; i < 8; i++) {
    DEV_Digital_Write(EPD_SCLK_PIN, 0);
    if (j & 0x80) {
      DEV_Digital_Write(EPD_MOSI_PIN, 1);
    } else {
      DEV_Digital_Write(EPD_MOSI_PIN, 0);
    }

    DEV_Digital_Write(EPD_SCLK_PIN, 1);
    j = j << 1;
  }
  DEV_Digital_Write(EPD_SCLK_PIN, 0);
  DEV_Digital_Write(EPD_CS_PIN, 1);
}

UBYTE DEV_SPI_ReadData() {
  UBYTE i, j = 0xff;
  DEV_GPIO_Mode(EPD_MOSI_PIN, 0);
  DEV_Digital_Write(EPD_CS_PIN, 0);
  for (i = 0; i < 8; i++) {
    DEV_Digital_Write(EPD_SCLK_PIN, 0);
    j = j << 1;
    if (DEV_Digital_Read(EPD_MOSI_PIN)) {
      j = j | 0x01;
    } else {
      j = j & 0xfe;
    }
    DEV_Digital_Write(EPD_SCLK_PIN, 1);
  }
  DEV_Digital_Write(EPD_SCLK_PIN, 0);
  DEV_Digital_Write(EPD_CS_PIN, 1);
  return j;
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI
protocol parameter: Info:
******************************************************************************/
UBYTE DEV_Module_Init(void) {
  printf("/***********************************/ \r\n");
  GPIO_Handle = lgGpiochipOpen(0);
  if (GPIO_Handle < 0) {
    Debug("gpiochip0 Export Failed\n");
    return -1;
  }
  DEV_GPIO_Init();
  printf("/***********************************/ \r\n");
  return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void) {
  DEV_Digital_Write(EPD_CS_PIN, 0);
  DEV_Digital_Write(EPD_PWR_PIN, 0);
  DEV_Digital_Write(EPD_DC_PIN, 0);
  DEV_Digital_Write(EPD_RST_PIN, 0);
  lgSpiClose(SPI_Handle);
  lgGpiochipClose(GPIO_Handle);
}
