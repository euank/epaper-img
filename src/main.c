#include "EPD_4in2b_V2.h"
#include "GUI_Paint.h"
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void Handler(int signo) {
  // System Exit
  printf("\r\nHandler:exit\r\n");
  DEV_Module_Exit();

  exit(0);
}

int main(int argc, char **argv) {
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
  Paint_SelectImage(BlackImage);

  int c;
  bool had_file = false;

  while (1) {
    static struct option long_options[] = {
        {"spidev", required_argument, 0, 0},
        {"pin-pwr", required_argument, 0, 0},
        {"pin-mosi", required_argument, 0, 0},
        {"pin-sclk", required_argument, 0, 0},
        {"pin-rst", required_argument, 0, 0},
        {"pin-dc", required_argument, 0, 0},
        {"pin-cs", required_argument, 0, 0},
        {"pin-busy", required_argument, 0, 0},
        {"file", required_argument, 0, 0},
        {0, 0, 0, 0}};
    int option_index = 0;
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (option_index) {
    case 0:
      // x.y, assume there's at most 9 devs and 9 chans
      if (strlen(optarg) != 3) {
        printf("--spidev takes an argument like 1.0\n");
        exit(1);
      }
      SPI_DEV = optarg[0] - '0';
      SPI_CHAN = optarg[2] - '0';
      break;
    case 1:
      EPD_PWR_PIN = atoi(optarg);
      break;
    case 2:
      EPD_MOSI_PIN = atoi(optarg);
      break;
    case 3:
      EPD_SCLK_PIN = atoi(optarg);
      break;
    case 4:
      EPD_RST_PIN = atoi(optarg);
      break;
    case 5:
      EPD_DC_PIN = atoi(optarg);
      break;
    case 6:
      EPD_CS_PIN = atoi(optarg);
      break;
    case 7:
      EPD_BUSY_PIN = atoi(optarg);
      break;
    case 8:
      GUI_ReadBmp(optarg, 10, 0);
      had_file = true;
      break;
    default:
      printf("unknown arg\n");
      exit(1);
    }
  }

  signal(SIGINT, Handler);
  if (DEV_Module_Init() != 0) {
    printf("error module init-ing\n");
    return -1;
  }

  EPD_4IN2B_V2_Init();
  if (!had_file) {
    printf("clear\n");
    EPD_4IN2B_V2_Clear();
    exit(0);
  }
  DEV_Delay_ms(500);

  EPD_4IN2B_V2_Display(BlackImage, RYImage);
  DEV_Delay_ms(2000);

  EPD_4IN2B_V2_Sleep();
  DEV_Delay_ms(2000);
  DEV_Module_Exit();

  return 0;
}
