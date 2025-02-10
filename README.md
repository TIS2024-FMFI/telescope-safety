# Telescope safety
Monitoring of current position of large astronomical telescope

## Descritption
The system monitors currrent position of the telescope. Based on the configuration (setup) the system logs the possitions to the log file.

When telescope enters forbiden zone the collision is always written to Collisions Log. (Even when logging is turned off.) The alarm is triggered if its turned on and motors are disabled if turning of motors is enabled.


All Changes to the configuration (setup) via webpaga are loged to Configuration Log.



## Hardware
### Inertial unit
- [Raspberry Pico 2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)
- [Inertial sensor](https://www.waveshare.com/wiki/10_DOF_IMU_Sensor_(D))
- [LoRa]()

![Inertial unit scheme](/docs/schemes/inertial_unit.png)

### Control unit
- [W55RP20-EVB-PICO](https://docs.wiznet.io/Product/ioNIC/W55RP20/w55rp20-evb-pico)
- [Arduino NANO]()
- [TLT LCD Shield Display]()
- Micro SD Card
- [LoRa]()
- 4x [Buttons]()
- 2x [Mosfet]()
- [Rele]()
- [Alarm]()

![Control unit scheme](/docs/schemes/control_unit.png)


## Manual
### Monitoring and confuguration website
On [main page](http://teleskop.local) there is displayed actual position of the telescope.

On [config page](http://teleskop.local/config) you can configurate forbidden zones, transformantion matrix, update and loging intervals or download logs.

When system starts you should calibrate actual azimut with display and buttons. The calibration is sent with long press of the button.



