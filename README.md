# telescope-safety
Monitoring of current position of large astronomical telescope

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
On [main page](teleskop.local) there is displayed actual position of the telescope.
On [config page](teleskop.local/config) you can configurate forbidden zones, transformantion matrix, update and loging intervals or download logs.


