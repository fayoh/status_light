# status_light [![Build Status](https://travis-ci.com/fayoh/status_light.svg?branch=master)](https://travis-ci.com/fayoh/status_light) [![Maintainability](https://api.codeclimate.com/v1/badges/9c8e848212beba8fd5fd/maintainability)](https://codeclimate.com/github/fayoh/status_light/maintainability) [![Test Coverage](https://api.codeclimate.com/v1/badges/9c8e848212beba8fd5fd/test_coverage)](https://codeclimate.com/github/fayoh/status_light/test_coverage)
Simple build status light from a toy traffic light and a serial connection to the computer.

This repo contains the firmware for the mcu in the traffic light and a
python client class implementing the serial protocol.

# HW #
The intended hw is an stm8s103 devboard with LEDs and a serial to usb
adapter connected.

# Building #

## Prerequisites ##
The fw requires a recent SDCC with stm8 support and the stm8 header from [stm8s-util](https://github.com/fayoh/stm8s-util)

To build both fw and the client simply issue make

``` shell
make
```
This will build a hex file with the firmware and a python wheel with the client library.

If you have stm8flash installed you can run ``` make flash ``` to flash the firmware.
