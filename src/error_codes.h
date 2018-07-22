/**
* This file is holding an enum with the different error codes.
*/

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

/**
* TODO: Collect all error codes that make sense
*/
typedef enum ERROR_CODE_T { _OK = 0, I2C_DEVICE_FAIL_TO_OPEN, IOCTL_ERROR, I2C_DEVICE_FAIL_READ, I2C_DEVICE_FAIL_WRITE} ERROR_CODE_T;


#endif /* ERROR_CODES_H */
