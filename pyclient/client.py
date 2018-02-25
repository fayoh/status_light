#!/usr/bin/env python3

# Depends on pyserial
import serial

# Bit 7-5: command
LIGHT_INTENSITY =(0x1 << 5)
LIGHT_BLINK     =(0x2 << 5)
LIGHT_PULSE     =(0x3 << 5)
MESSAGE_LINE_1  =(0x4 << 5)
MESSAGE_LINE_2  =(0x5 << 5)

# Bit 4-0: data
RED    =0x1
ORANGE =0x2
GREEN  =0x4

CMD_MASK  =0xE0
DATA_MASK =0x1F

port = serial.Serial("/dev/ttyUSB0", baudrate=9600,timeout=1)

a=(LIGHT_INTENSITY|ORANGE)<<8|0x80
port.write(a.to_bytes(2,'big'))
print(port.read(2))
a=(LIGHT_INTENSITY|GREEN)<<8|0x50
port.write(a.to_bytes(2,'big'))
a=(LIGHT_INTENSITY|RED)<<8|0xe0
port.write(a.to_bytes(2,'big'))

#a=(LIGHT_PULSE|GREEN)<<8|0x0
#port.write(a.to_bytes(2,'big'))
#a=(LIGHT_PULSE|ORANGE)<<8|0x0
#port.write(a.to_bytes(2,'big'))
#a=(LIGHT_PULSE|RED)<<8|0x1
#port.write(a.to_bytes(2,'big'))

port.close()
