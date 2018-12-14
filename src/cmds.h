#ifndef CMDS_H
#define CMDS_H

// Bit 7-5: command
#define LIGHT_INTENSITY (0x1 << 5)
#define LIGHT_BLINK     (0x2 << 5)
#define LIGHT_PULSE     (0x3 << 5)
#define MESSAGE_LINE_1  (0x4 << 5)
#define MESSAGE_LINE_2  (0x5 << 5)

// Bit 4-0: data
#define RED   0x1
#define ORANE 0x2
#define GREEN 0x3

#define CMD_MASK  0xE0
#define DATA_MASK 0x1F

#endif //CMDS_H
