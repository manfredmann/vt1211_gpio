/*
 * GPIO Userspace library for VT1211 Super I/O chip
 *
 * Copyright 2018 by Roman Serov <roman@serov.co>
 * 
 * This file is part of VT1211 GPIO Userspace library.
 *
 * VT1211 GPIO Userspace library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * VT1211 GPIO Userspace library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with VT1211 GPIO Userspace library. If not, see <http://www.gnu.org/licenses/>.
 * 
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/

#ifndef VT1211_GPIO_H
#define VT1211_GPIO_H

#include <stdint.h>
#include <stdbool.h>

#include <sys/io.h>

#ifdef __QNXNTO__

  //QNX 6.5
  #include <hw/inout.h>
  #include <sys/neutrino.h>
  
  #define io_write8(addr, data)   out8(addr, data)
  #define io_read8(data)          in8(data)
  #define io_request()            ~ThreadCtl(_NTO_TCTL_IO, NULL)

#elif __gnu_linux__

  //GNU/Linux
  #define io_write8(addr, data)   outb(data, addr)
  #define io_read8(addr)          inb(addr)
  #define io_request()            ~iopl(3)
  
#else

  #error Unsupported OS

#endif

#define VT_INIT_OK                    0x00
#define VT_INIT_NOT_FOUND             0x01
#define VT_INIT_NO_PORT               0x02

#define VT_ENTER_CONFIG               0x87
#define VT_EXIT_CONFIG                0xAA

#define VT_CONFIG_LDN                 0x07
#define VT_CONFIG_DEV_ID              0x20
#define VT_CONFIG_DEV_REV             0x21
#define VT_CONFIG_GPIO_P1_PIN_SELECT  0x24
#define VT_CONFIG_GPIO_P2_PIN_SELECT  0x25

#define VT_CONFIG_PORT_1              0x01
#define VT_CONFIG_PORT_3_6            0x04

#define VT_LDN_GPIO                   0x08

#define VT_LDN_GPIO_ACTIVATE          0x30
#define VT_LDN_GPIO_BADDR_B1          0x60
#define VT_LDN_GPIO_BADDR_B2          0x61
#define VT_LDN_GPIO_PORT_SELECT       0xF0
#define VT_LDN_GPIO_MODE_CONFIG       0xF1
#define VT_LDN_GPIO_POLAR_CONFIG      0xF2

#define VT_PORT_1                     0x00 //GP10...GP17
#define VT_PORT_3                     0x01 //GP30...GP37
#define VT_PORT_4                     0x02 //GP40...GP47
#define VT_PORT_5                     0x03 //GP50...GP57
#define VT_PORT_6                     0x04 //GP60...GP62

#define VT_PORT_INPUT                 0x00
#define VT_PORT_OUTPUT                0xFF

#define VT_PORT_PL_DIRECT             0x00
#define VT_PORT_PL_INVERSE            0xFF

#define VT_PIN_INPUT                  0x1
#define VT_PIN_OUTPUT                 0x0

#define VT_PIN_PL_DIRECT              0x0
#define VT_PIN_PL_INVERSE             0x1

#define VT_PIN_0                     (0x1)
#define VT_PIN_1                     (0x1 << 1)
#define VT_PIN_2                     (0x1 << 2)
#define VT_PIN_3                     (0x1 << 3)
#define VT_PIN_4                     (0x1 << 4)
#define VT_PIN_5                     (0x1 << 5)
#define VT_PIN_6                     (0x1 << 6)
#define VT_PIN_7                     (0x1 << 7)

int       vt_init(uint8_t ports, uint16_t cir, uint16_t cdr);
uint8_t   vt_get_dev_id();
uint8_t   vt_get_dev_rev();
uint16_t  vt_get_baddr();

void      vt_port_mode(uint8_t port, uint8_t mode);
void      vt_port_polarity(uint8_t port, uint8_t polarity);
void      vt_port_write(uint8_t port, uint8_t data);
uint8_t   vt_port_read(uint8_t port);

void      vt_pin_mode(uint8_t port, uint8_t pin, uint8_t mode);
void      vt_pin_polarity(uint8_t port, uint8_t pin, uint8_t polarity);
void      vt_pin_set(uint8_t port, uint8_t pin, uint8_t value);
uint8_t   vt_pin_get(uint8_t port, uint8_t pin);
#endif
