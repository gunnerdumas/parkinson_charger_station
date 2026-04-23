#ifndef BQ25890_DRIVER_H
#define BQ25890_DRIVER_H

/*
Driver for the BQ25890 TI sys lipo charger used in an stm32 enviroment part
of the parkinson charger

Author: Gunner Cook-Dumas
Date created: 4/20/26
*/

//target address 1101010+r/w(1/0)
#define BQ25890_ADDR (0b1101010 << 1)

//Registers
//The charger device supports multi-read and multi-write on REG00 through REG14 except REG0C.
//REG0C is a fault register.

#define BQ25890_REG00 0x00
#define BQ25890_REG01 0x01
#define BQ25890_REG02 0x02
#define BQ25890_REG03 0x03
#define BQ25890_REG04 0x04
#define BQ25890_REG05 0x05
#define BQ25890_REG06 0x06
#define BQ25890_REG07 0x07
#define BQ25890_REG08 0x08
#define BQ25890_REG09 0x09
#define BQ25890_REG0A 0x0A
#define BQ25890_REG0B 0x0B
#define BQ25890_REG0C 0x0C
#define BQ25890_REG0D 0x0D
#define BQ25890_REG0E 0x0E
#define BQ25890_REG0F 0x0F
#define BQ25890_REG10 0x10
#define BQ25890_REG11 0x11
#define BQ25890_REG12 0x12
#define BQ25890_REG13 0x13
#define BQ25890_REG14 0x14

int initBQ25890();




#endif BQ25890_DRIVER_H
