/**
* Configures and collect the results of teh VEML6040
**/

#define DEBUG

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
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


int8_t gain_calc(uint16_t value) {
  uint8_t temp;
  int8_t result = -1;

  temp = (value & 0xf000) >> 12 | 1; /* | 1 to prevent 0 remaining in var -> inifinite loop in while */

  while (temp < 0x0f) {
    temp = temp << 1;
    result++;
  }

  return result;
}


int main(int arcc, char **argv) {
  ERROR_CODE_T ret_val;
  uint16_t value;
  uint8_t gain[4] = {3, 3, 3, 3};
  uint16_t color_val[4];
  int8_t exp_change;
  uint8_t color;
  uint8_t temp;

  ret_val = veml6040_init();

  /*get initial exposure time, sequential all channels */
  for (color = 0; color < 4; color++) {
    exp_change = 1;
    while (exp_change !=0 ) {
      i2c_write_byte(VEML6040_OFFSET_CONF, (gain[color]<<8) + VEML6040_CONF_AF + VEML6040_CONF_TRIG);
      usleep(40000 << gain[color]);
      i2c_read(VEML6040_OFFSET_R_DATA + color, &value);
      exp_change = gain_calc(value);
      if ((gain[color] + exp_change) > 5) {
        gain[color] = 5;
        break;
      } else if ((gain[color] + exp_change) < 0) {
        gain[color] = 0;
        break;
      } else {
        gain[color] +=exp_change;
      }
    }

    i2c_write_byte(VEML6040_OFFSET_CONF, (gain[color]<<4) + VEML6040_CONF_AF + VEML6040_CONF_TRIG);
    usleep(40000 << gain[color]);
    i2c_read(VEML6040_OFFSET_R_DATA + color, &value);
    color_val[color] = value;
  }

  printf("Initial Gain values: R:     %d, G:     %d, B:     %d, W:     %d\n", gain[0], gain[1], gain[2], gain[3]);
  printf("initial luminosity:  R:0x%04x, G:0x%04x, B:0x%04x, W:0x%04x\n", color_val[0], color_val[1], color_val[2], color_val[3]);

return 0;
//  i2c_write_byte(VEML6040_OFFSET_CONF, 0x50 + VEML6040_CONF_AF + VEML6040_CONF_TRIG);
//  sleep(2);
//  i2c_read(VEML6040_OFFSET_R_DATA, &nvalue);
//  printf("R_Value = %04x\n", nvalue);
//
//  i2c_write_byte(VEML6040_OFFSET_CONF, 0x40 + VEML6040_CONF_AF + VEML6040_CONF_TRIG);
//  sleep(1);
//  i2c_read(VEML6040_OFFSET_R_DATA, &nvalue);
//  printf("R_Value = %04x\n", nvalue);
//
//  return ret_val;
//
//  while(1) {
//    i2c_read(VEML6040_OFFSET_R_DATA, &nvalue);
//    printf("R_Value = %04x", nvalue);
//    i2c_read(VEML6040_OFFSET_G_DATA, &nvalue);
//    printf(" | G_Value = %04x", nvalue);
//    i2c_read(VEML6040_OFFSET_B_DATA, &nvalue);
//    printf(" | B_Value = %04x\n", nvalue);
//  }
//
//  return ret_val;
}
