# ESP-Homekit-ACController
an Homekit AC controller base on ESP8266


## Apple HomeKit AC controller for ESP8266 Arduino
This project is base on the sdk provided by [Mixiaoxiao/Arduino-HomeKit-ESP8266](https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266/), so you need install the libraries first.


Mixiaoxiao provided the fireware file of the AC controller, which is pretty and useful. But I have a special hardware need change something before send the ir code, I have to write the source code myself. That's why this project was born.


## Setup code of the example sketch

``111-11-111``

## Usage
* change the ssid and password in wifi_info.h
* change the AC type in ESP-Homekit-ACContoller.ino in line 57, default is KELVINATOR (Gree default protocol)


## Todo
* add configuration webpage to change the AC type  with change the source code.
* add configuration webpage to change gpio pin code
* add wifimanager support

## Thanks
* Mixiaoxiao [Arduino-HomeKit-ESP8266](https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266/)
* maximkulkin [esp-homekit](https://github.com/maximkulkin/esp-homekit)
* maximkulkin [esp-homekit-demo](https://github.com/maximkulkin/esp-homekit-demo)
* bznsix [bznsix](https://github.com/bznsix/8266IrRemote)