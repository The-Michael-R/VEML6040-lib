/**
* Register definitions for VEML6040 RGBW light sensor
* Datasheet: http://www.vishay.com/docs/84276/veml6040.pdf
**/

#ifndef VEML6040_h
#define VEML6040_h

#include "stdint.h"
#include "error_codes.h"
#include "global_conf.h"
#include "debug_util.h"

/**
* VEML6040 slave address
**/
#define VEML6040_I2C_ADDESS (0x10)


/**
* Offset definitions of registers in VEML6040
**/
#define VEML6040_OFFSET_CONF   (0x00)
#define VEML6040_OFFSET_R_DATA (0x08)
#define VEML6040_OFFSET_G_DATA (0x09)
#define VEML6040_OFFSET_B_DATA (0x0a)
#define VEML6040_OFFSET_W_DATA (0x0b)


/**
* Position and length of control bits in CONF Register
*   CONF[6:4]  IT    Integration time setting
*   CONF[2]    TRIG  Proceed one detecting cycle at manual force mode
*   CONF[1]    AF    Auto / manual force mode
*   CONF[0]    SD    Chip shutdown setting
*
*
* IT definition
* Integration time of sensor
* IT = 0b000 ->   40ms
* IT = 0b001 ->   80ms
* IT = 0b010 ->  160ms
* IT = 0b011 ->  320ms
* IT = 0b100 ->  640ms
* IT = 0b101 -> 1280ms
*
* TRIG definition
* TRIG = 0 -> No trigger
* TRIG = 1 -> Trigger one time detect cycle
*
* AF definition
* AF = 0 -> Auto mode (continious measurement)
* AF = 1 -> Force mode (single measurement)
*
* SD definition
* SD = 0 -> Normal operation
* SD = 1 -> Shut Down mode
**/
#define VEML6040_CONF_IT     (4)
#define VEML6040_CONF_IT_MSK (111 << VEML6040_CONF_IT)
#define VEML6040_CONF_TRIG   (2)
#define VEML6040_CONF_AF     (1)
#define VEML6040_CONF_SD     (0)

#define LUX_PER_STEP 0.007865


/**
* Public functions
**/
ERROR_CODE_T veml6040_init(void); /* initializes the i2c and teh veml6040 */
ERROR_CODE_T sample_veml(uint32_t *raw_rgbw); /* reading all channels */
void convert_luminance(const uint32_t *raw_rgbw, double *lux_rgbw); /* convert to lux */

#endif /* VEML6040_h */
