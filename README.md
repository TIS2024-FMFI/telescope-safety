# Telescope safety
Monitoring of current position of large astronomical telescope

## Descritption
The system monitors currrent position of the telescope. Based on the configuration (setup) the system logs the possitions to the log file.

When telescope enters forbiden zone the collision is always written to Collisions Log. (Even when logging is turned off.) The alarm is triggered if its turned on and motors are disabled if turning of motors is enabled.

When telescope leaves the forbidden zone alarm is automatically turned off and motors reanabled.


All Changes to the configuration (setup) via webpaga are loged to Configuration Log.



## Hardware
### Inertial unit
- [Raspberry Pico 2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)
- Inertial sensor (alternatives)
  - [Waveshare 10 DOF IMU Sensor (D), ICM20948 Onboard](https://www.sparkfun.com/sparkfun-6dof-imu-breakout-ism330dhcx-qwiic.html)
  - [SparkFun 6DoF IMU Breakout - ISM330DHCX](https://www.sparkfun.com/sparkfun-6dof-imu-breakout-ism330dhcx-qwiic.html) (best choice)
  - [SparkFun 9DoF IMU Breakout - ICM-20948](https://www.sparkfun.com/sparkfun-9dof-imu-breakout-icm-20948-qwiic.html)
- LoRa 868MHz SX1276 RF Transreceiver Module RFM96W

![Inertial unit scheme](/docs/schemes/inertial_unit.png)

### Control unit
- [W55RP20-EVB-PICO](https://docs.wiznet.io/Product/ioNIC/W55RP20/w55rp20-evb-pico)
- [Arduino NANO](https://store.arduino.cc/en-sk/products/arduino-nano)
- 3.5 TLT LCD Shield Display
- Micro SD Card
- LoRa 868MHz SX1276 RF Transreceiver Module RFM96W
- 4x Kapacitné dotykové tlačidlo TTP223
- 2x MosfetDual mosfet PWM regulátor AOD4184A
- Relé modul 1 kanál 5V
- Siréna 12V LS-103

![Control unit scheme](/docs/schemes/control_unit.png)


## Manual
### Monitoring and confuguration website
On [main page](http://teleskop.local) there is displayed actual position of the telescope.

On [config page](http://teleskop.local/config) you can configurate forbidden zones, transformantion matrix, update and loging intervals or download logs.

When system starts you should calibrate actual azimut with display and buttons. The calibration is sent with long press of the button.

In "libraries" folders for each unit you have instructions about external libraries used.

