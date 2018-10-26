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

21:50:43 26.09.2018, 0x000009, 0x000000, 0x000006, 0x000010, 0.070785, 0.000000, 0.047190, 0.125840
...
```
Yes it was dark at that time ;-)

If you want to log the brightness over a longer time you can use `./rgbw-logger > logging-data.txt & disown -h` to run the code in background and you can even log off from the SSH while the code continues in the background. If you want to stop the code just kill the process (`killall rgbw-logger`). Be aware that there is a buffer for the piping. So it takes a while before the first data will appear in the file and if you kill the process not all data might be present in the file. So be patient and let it run some additional time.

## ToDo
There is a strange behavior which seems to be HW related and I think the sensor is the culprit. This problem is solved by adding more than required delays after trigger the measurement.
Adding a description how to setup this on the RaspberryPi and Arch-Linux.
