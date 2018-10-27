# VEML6040-lib
## What is this code for?
This code can be used to access the VEML6040 form Vishay Semiconductors. This is a private project and not related to Vishay Semiconductors.

This code comes with an example which prints out the data from the sensor every 10 seconds.


## make options
How to build something
- `make` builds the example
- `make debug` builds the example with enabled debug printing
- `make doc` builds the example and the documentation (./doc)
- `make clean` removes all binaries


## Requirements
Good question. I run the code on a RaspberryPi with Arch-Linux. This requires the i2c to be enabled
and the related modules installed.

To generate the documentation doxygen is required.


## Usage
After connecting the sensor to the RaspberryPi, enable the i2c in the Linux distribution just build and start the program.
```
# ./rgbw-logger

16:40:50 27.09.2018, 0x0292a4, 0x028ef4, 0x01c346, 0x063da8, 1326.133 lux, 1318.709 lux,  908.612 lux, 3216.785 lux
16:41:02 27.09.2018, 0x027b5c, 0x027ba8, 0x01b702, 0x060158, 1279.258 lux, 1279.856 lux,  883.916 lux, 3095.349 lux
16:41:14 27.09.2018, 0x026b40, 0x026e64, 0x01b03e, 0x05ddf8, 1246.823 lux, 1253.146 lux,  870.294 lux, 3024.124 lux
16:41:26 27.09.2018, 0x026f6c, 0x0270fc, 0x01b164, 0x05e6d8, 1255.223 lux, 1258.369 lux,  872.606 lux, 3041.993 lux
...
```
If you want to log the brightness over a longer time you can use `./rgbw-logger > logging-data.txt & disown -h` to run the code in background and you can even log off from the SSH while the code continues in the background. If you want to stop the code just kill the process (`killall rgbw-logger`). Be aware that there is a buffer for the piping. So it takes a while before the first data will appear in the file and if you kill the process not all data might be present in the file. So be patient and let it run some additional time.

## ToDo
- There is a strange behavior which seems to require some additional time after trigger the device.
- Adding a description how to setup this on the RaspberryPi and Arch-Linux.
