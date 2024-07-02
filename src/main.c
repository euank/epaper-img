#include "EPD_Test.h" //Examples
#include <signal.h>   //signal()
#include <stdlib.h>   //exit()

void Handler(int signo) {
  // System Exit
  printf("\r\nHandler:exit\r\n");
  DEV_Module_Exit();

  exit(0);
}

int main(void) {
  signal(SIGINT, Handler);
  EPD_4in2b_V2_test();
  return 0;
}
