/**
* A library I wrote for the  VEML6040 sensor from Vishay Semiconductors.
* This is a private project and not related to Vishay Semiconductors.
*
* Configures and collect the results of the VEML6040
*
* Providing the RAW values as from the sensor and the function to convert this
* values to LUX.
*
* Public functions:
*   int veml6040_init()
*   ERROR_CODE_T sample_veml(uint32_t *raw_rgbw)
*   void convert_luminance(const uint32_t *raw_rgbw, float *lux_rgbw)
**/

#include <fcntl.h> /* open */
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h> /* usleep */

#include "veml6040.h"

/**
* Private stuff
**/
int i2c_file_handle;
ERROR_CODE_T i2c_read(const uint8_t addr, uint16_t* data);
ERROR_CODE_T i2c_write_byte(const uint8_t addr, const uint8_t data);


/**
* Initialízes the i2c interface and the veml6040 device
* @public
**/
ERROR_CODE_T veml6040_init(void) {
  i2c_file_handle = open (I2C_DEVICE, O_RDWR);

  if (i2c_file_handle < 0) {
    DEBUG_PRINT("ERROR: I2C_DEVICE_FAIL_TO_OPEN\n");
    return I2C_DEVICE_FAIL_TO_OPEN;
  }

  return _OK;
}

/**
* Read 16bit (word) from i2c device from provided address. This code uses the
* correct approach of reading the value. Split this to two i2c commands isn't
* valid and the veml6040 won't respond to that construct. So the ioctl() is used.
*
* @param   addr  Address of word to be read from i2c device
* @param  *data  Pointer to uint16_t memory. If i2c read was successful data will
*                be updated. Otherwise the value will be unchanged.
* @return Error code as defined in error_codes.h
* @private
**/
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
      DEBUG_PRINT("ERROR: In ioctl i2c_read");
      return I2C_DEVICE_FAIL_READ;
  }

  *data = (input_buffer[1] << 8) + input_buffer[0];
  return _OK;
}

/**
* Writing 16bit (word) to the i2c device.
*
* @param  addr  Address of word to be written to i2c device
* @param  data  Data of uint16_t that will be written to i2c device
* @return Error code as defined in error_codes.h
* @private
**/
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
    DEBUG_PRINT("ERROR: In ioctl i2c_write");
    return I2C_DEVICE_FAIL_WRITE;
  }

  return _OK;
}


/**
* Read all four channels form veml6040. To get the full dynamic range of the
* device this code tries the shortest integration time and increases until all
* channels are in the optimal range or the longest integration tie is reached.
*
* @TODO: There is a unclear addtitional delay required @ gain 1..3 while 0 and 4
* and 5 work with the expeted 40ms per gain plus 10ms.
*
* @param raw_rgbw Pointer to array of uint32_t. This array will hold the current
*                 values of lumince in the order of red, green, blue, white.
*                 Each step represents 0.007865 Lux.
* @public
**/
ERROR_CODE_T sample_veml(uint32_t *raw_rgbw) {
  ERROR_CODE_T error_code;
  uint8_t curr_gain;
  uint8_t color;
  uint8_t channels_done;
  uint16_t value;

  channels_done = 0;

  for(curr_gain = 0; curr_gain < 6; curr_gain++) {

    error_code = i2c_write_byte(VEML6040_OFFSET_CONF, (curr_gain << VEML6040_CONF_IT) + (1 << VEML6040_CONF_AF) + (1 << VEML6040_CONF_TRIG));
    if (error_code) {
      return error_code;
    }
    usleep((60000 << curr_gain) + 10000); /* 40ms for gain = 0 plus some margin */

    for (color = 0; color < 4; color++) {
      if ((channels_done & (1 << color)) == 0) { /* read only the not finished channels */
        error_code = i2c_read(VEML6040_OFFSET_R_DATA + color, &value);
        if (error_code) {
          return error_code;
        }

        raw_rgbw[color] = value << (5 - curr_gain);

        if (value > 0x7fff) {
          channels_done = channels_done | (1 << color);

          if (channels_done == 0b1111) {
            /* if all channels are done, do an early exit */
            return _OK;
          }
        }
      }
    }
  }

return _OK;
}

/**
* Convert the RAW values to lux. It simply multiplies the RAW value with 0.007865
*
* @param const raw_rgbw Array that holds the RAw values provided from the function
*                       sample_veml
* @param *lux_rgbw      Pointer to an array of floats. This array will have the
*                       values in lux after the function call.
* @public
**/
void convert_luminance(const uint32_t *raw_rgbw, double *lux_rgbw) {
  uint8_t color;

  for (color=0; color<4; color++) {
    lux_rgbw[color] = raw_rgbw[color] * LUX_PER_STEP;
  }

}
