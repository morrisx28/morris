# STMICROELECTRONICS - VL53L5CX Linux driver
Official VL53L5CX Linux driver and test applications for linux and android platforms

## Updates
The following changes have been applied to the official API according to the discussion in [link](https://community.st.com/s/question/0D53W00001GQQVaSAP/connect-multiple-vl53l5cx-on-the-same-i2c-bus-of-jetson-nano).
- Updated the `write_read_multi()` function in `platform.cpp` file. Earlier on, it used the address of the sensor as 0x29, even if it was detected on some other bus. But now it has been fixed.
- Updated the `vl53l5cx_comms_init(VL53L5CX_Platform * p_platform)` to `vl53l5cx_comms_init(VL53L5CX_Platform * p_platform, char * device, uint16_t address)`, where: -

    - `device`: Address of the device eg. `"/dev/i2c-1"`. Earlier on, it was hardcoded to `"/dev/i2c-1"` within this function.
    - `address`: The I2C address of the sensor. Earlier on it was assumed that the address was `0x29` but now it is configuraable.
- Updated the `vl53l5cx_set_i2c_address(VL53L5CX_Configuration *p_dev, uint16_t i2c_address)` function in `vl53l5cx_api.cpp` so that it actually sets the address that is passed to it instead of any bit shifing. Earlier on, lets say if we pass it to set the address to `0x40`, it used to set the address to `0x20`.
- Updated the `WaitMs(VL53L5CX_Configuration *p_dev, uint32_t time_ms)` to `WaitMs(uint32_t time_ms)`, since the other argument was not needed within the function.


## Introduction
The proposed implementation is customized to run on a raspberry pi v3, but can be adapted to run on any linux embedded platform,
as far as the VL53L5CX device is connected through I2C
Two options are offered to the user
- 1. compile and run this driver with a kernel module responsible for handling i2c bus and the interruption. This is the kernel mode
- 2. compile and run this driver in a full user mode, where the i2c commnication is handled with the /dev/i2c-1 file descriptor. This is the user mode

Option 1 supports the interruption line of the VL53L5CX but needs a kernel module to be compiled and inserted.
Option 2 may be more suitable for simple application, but needs the /dev/i2c-1 to be available which may not be the case on some secured platforms

## How to run a test application on raspberry pi
    Note that the following instructions were tested on raspberrypi 3.

### Install the raspberry pi kernel source headers (kernel mode only)
    refer to raspberrypi official documentation to download the headers matching your kernel version
    $ sudo apt-get install raspberrypi-kernel-headers

### update /boot/config.txt file (kernel mode only)
    $ sudo nano /boot/config.txt
    --> add or uncomment the following lines at the end of the /boot/config.txt
    dtparam=i2c_arm=on
    dtparam=i2c1=on
    dtparam=i2c1_baudrate=1000000
    dtoverlay=stmvl53l5cx
### compile the device tree blob (kernel mode only)
    $ cd kernel
    $ make dtb
    $ sudo reboot
### compile the test examples, the platform adaptation layer and the uld driver
    $ nano user/test/Makefile
    Enable or disable the STMVL53L5CX_KERNEL cflags option depending on the wished uld driver mode : with a kernel module of fully in user.
    $ cd vl53l5cx-driver/user/test
    $ make
### compile the kernel module (kernel mode only)
    $ cd kernel
    $ make clean
    $ make
    $ sudo make insert
### run the test application menu
    $ cd user/test
    $ ./menu


