/**
* Configures and collect the results of the VEML6040
**/

#define DEBUG

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <pthread.h>
#include "veml6040.h"
#include "global_conf.h"
#include "error_codes.h"
#include "debug_util.h"
#include <time.h>

/**
* Private stuff
**/
int i2c_file_handle;


int veml6040_init() {
  int status;
  i2c_file_handle = open (I2C_DEVICE, O_RDWR);

  if (i2c_file_handle < 0) {
    DEBUG_PRINT("ERROR: I2C_DEVICE_FAIL_TO_OPEN\n")
    return I2C_DEVICE_FAIL_TO_OPEN;
  }

  return _OK;
}


ERROR_CODE_T i2c_read(const uint8_t addr, uint16_t* data) {
  uint8_t output_buffer[1];
  uint8_t input_buffer[2];

  struct i2c_rdwr_ioctl_data msgset;
  struct i2c_msg msgs[2];

  output_buffer[0] = addr;

  msgs[0].addr = VEML6040_I2C_ADDESS;
  msgs[0].flags = 0;
  msgs[0].len = 1;
  msgs[0].buf = output_buffer;

  msgs[1].addr = VEML6040_I2C_ADDESS;
  msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
  msgs[1].len = 2;
  msgs[1].buf = input_buffer;

  msgset.msgs = msgs;
  msgset.nmsgs = 2;

  if (ioctl(i2c_file_handle, I2C_RDWR, &msgset) < 0) {
      printf("ERROR: In ioctl i2c_read");
      return I2C_DEVICE_FAIL_READ;
  }

  *data = (input_buffer[1] << 8) + input_buffer[0];
  return _OK;
}


ERROR_CODE_T i2c_write_byte(const uint8_t addr, const uint8_t data) {
  uint8_t output_buffer[2];

  struct i2c_rdwr_ioctl_data msgset;
  struct i2c_msg msgs[1];

  output_buffer[0] = addr;
  output_buffer[1] = data;

  msgs[0].addr  = VEML6040_I2C_ADDESS;
  msgs[0].flags = 0;
  msgs[0].len   = 2;
  msgs[0].buf   = output_buffer;

  msgset.msgs  = msgs;
  msgset.nmsgs = 1;

  if (ioctl(i2c_file_handle, I2C_RDWR, &msgset) < 0) {
    printf("ERROR: In ioctl i2c_write");
    return I2C_DEVICE_FAIL_WRITE;
  }

  return _OK;
}


void sample_veml() {
  uint8_t curr_gain;
  uint16_t color_val[4];
  uint8_t  color_gain[4];
  uint8_t color;
  uint8_t channels_done;
  uint16_t i;

  time_t now;
  struct tm *datetime;

  curr_gain = 0;
  channels_done = 0;
  for (color=0; color<4; color++) {
    color_gain[color] = 5;
  }

  do {
    /* start converting */
    i2c_write_byte(VEML6040_OFFSET_CONF, (curr_gain << VEML6040_CONF_IT) + (1 << VEML6040_CONF_AF) + (1 << VEML6040_CONF_TRIG));
    usleep(40000 << curr_gain);
    usleep(1000); /*add some additional time */

    for (color = 0; color < 4; color++) {
      if (color_gain[color] == 5) {
        i2c_read(VEML6040_OFFSET_R_DATA + color, &color_val[color]);
        if (color_val[color] > 0x3fff) {
          color_gain[color] = curr_gain;
          channels_done ++;
        }
      }
    }
    curr_gain++;
    if (curr_gain > 5) {
      break;
    }
  }  while (channels_done != 4);

  time(&now);
  datetime = localtime(&now);
  printf("\n%02i:%02i:%02i %02i.%02i.%04i", datetime->tm_hour, datetime->tm_min, datetime->tm_sec, datetime->tm_mday, datetime->tm_mon, datetime->tm_year + 1900);
  for (color = 0; color < 4; color++) {
    printf (", %01d, 0x%04x", color_gain[color], color_val[color]);
  }
}


void *threadproc(void *arg)
{
    while(1)
    {
        sample_veml();
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
