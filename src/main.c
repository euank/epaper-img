#include "EPD_4in2b_V2.h"
#include "GUI_Paint.h"
#include <signal.h>
#include <stdlib.h>
#include <time.h>

void Handler(int signo) {
  // System Exit
  printf("\r\nHandler:exit\r\n");
  DEV_Module_Exit();

  exit(0);
}

int main(void) {
  signal(SIGINT, Handler);
  if (DEV_Module_Init() != 0) {
    return -1;
  }

  printf("e-Paper Init and Clear...\r\n");
  EPD_4IN2B_V2_Init();
  EPD_4IN2B_V2_Clear();
  DEV_Delay_ms(500);

  // Create a new image cache named IMAGE_BW and fill it with white
  UBYTE *BlackImage, *RYImage; // Red or Yellow
  UWORD Imagesize =
      ((EPD_4IN2B_V2_WIDTH % 8 == 0) ? (EPD_4IN2B_V2_WIDTH / 8)
                                     : (EPD_4IN2B_V2_WIDTH / 8 + 1)) *
      EPD_4IN2B_V2_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for black memory...\r\n");
    return -1;
  }
  if ((RYImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for red memory...\r\n");
    return -1;
  }
  printf("NewImage:BlackImage and RYImage\r\n");
  Paint_NewImage(BlackImage, EPD_4IN2B_V2_WIDTH, EPD_4IN2B_V2_HEIGHT, 180,
                 WHITE);
  Paint_NewImage(RYImage, EPD_4IN2B_V2_WIDTH, EPD_4IN2B_V2_HEIGHT, 180, WHITE);

  for (unsigned int i = 0; i < EPD_4IN2B_V2_WIDTH / 8; i++) {
    for (unsigned int j = 0; j < EPD_4IN2B_V2_HEIGHT; j++) {
      unsigned int Addr = i + j * EPD_4IN2B_V2_WIDTH / 8;
      BlackImage[Addr] = WHITE;
      RYImage[Addr] = WHITE;
    }
  }

  printf("show window BMP-----------------\r\n");
  Paint_SelectImage(BlackImage);
  GUI_ReadBmp("./pic/100x100.bmp", 10, 0);
  EPD_4IN2B_V2_Display(BlackImage, RYImage);
  DEV_Delay_ms(2000);

  printf("Clear...\r\n");
  EPD_4IN2B_V2_Clear();

  printf("Goto Sleep...\r\n");
  EPD_4IN2B_V2_Sleep();
  free(BlackImage);
  free(RYImage);
  DEV_Delay_ms(2000); // important, at least 2s
  // close 5V
  printf("close 5V, Module enters 0 power consumption ...\r\n");
  DEV_Module_Exit();

  return 0;
}
