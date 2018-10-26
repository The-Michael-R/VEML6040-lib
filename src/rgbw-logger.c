/**
* This code can be seen as example code for the library I wrote for the
* VEML6040 sensor from Vishay Semiconductors. This is a private project and
* not related to Vishay Semiconductors.
*
* Using the veml6040 library to collect the luminance on all four channels
* (RGB and W) and print the values out every 10 seconds.
*
* This code runs until stopped (e.g. ctrl-c).
**/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h> /* sleep */
#include <time.h>

#include "veml6040.h"


/* Prototypes */
void print_luminance(void);
void *threadproc(void *arg);
int main(int arcc, char **argv);


void print_luminance(void) {
  uint32_t color_value[4];
  double color_lux[4];
  uint8_t color;

  sample_veml(color_value);

  time_t now;
  struct tm *datetime;

  time(&now);
  datetime = localtime(&now);
  printf("\n%02i:%02i:%02i %02i.%02i.%04i", datetime->tm_hour, datetime->tm_min, datetime->tm_sec, datetime->tm_mday, datetime->tm_mon, datetime->tm_year + 1900);
  for (color = 0; color < 4; color++) {
    printf (", 0x%06x", color_value[color]);
  }
  convert_luminance(color_value, color_lux);
  for (color = 0; color < 4; color++) {
    printf (", %f", color_lux[color]);
  }

}


void *threadproc(void *arg)
{
    while(1)
    {
        print_luminance();
        sleep(10);
    }
    return 0;
}


int main(int arcc, char **argv) {
  pthread_t tid;
  veml6040_init();

  pthread_create(&tid, NULL, &threadproc, NULL);
  while (pthread_join(tid,NULL)) {};

  printf("\n");

  return 0;
}
