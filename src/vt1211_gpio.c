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

#include "vt1211_gpio.h"

static struct {
  uint16_t  baddr;
  uint8_t   dev_id;
  uint8_t   dev_rev;
  uint16_t  cir;
  uint16_t  cdr;
} gpio_data;

void vt_config_mode() {
  io_write8(gpio_data.cir, VT_ENTER_CONFIG);
  io_write8(gpio_data.cir, VT_ENTER_CONFIG);
}

void vt_config_mode_exit() {
  io_write8(gpio_data.cir, VT_EXIT_CONFIG);
}

void vt_config_mode_gpio() {
  io_write8(gpio_data.cir, VT_CONFIG_LDN);
  io_write8(gpio_data.cdr, VT_LDN_GPIO);
}

uint16_t vt_get_id_rev() {
  vt_config_mode();

  io_write8(gpio_data.cir, VT_CONFIG_DEV_ID);
  uint8_t id = io_read8(gpio_data.cdr);

  io_write8(gpio_data.cir, VT_CONFIG_DEV_REV);
  uint8_t rev = io_read8(gpio_data.cdr);

  vt_config_mode_exit();

  return ((id << 8) | rev);
}

void vt_port_as_gpio(uint8_t port) {
  vt_config_mode();

  switch(port) {
    case VT_CONFIG_PORT_1: {
      io_write8(gpio_data.cir, VT_CONFIG_GPIO_P1_PIN_SELECT);
      io_write8(gpio_data.cdr, 0xFF);
    }
    break;
    case VT_CONFIG_PORT_3_6: {
      io_write8(gpio_data.cir, VT_CONFIG_GPIO_P2_PIN_SELECT);
      uint8_t d_tmp = io_read8(gpio_data.cdr);
      d_tmp |= 0b00111111;

      io_write8(gpio_data.cir, VT_CONFIG_GPIO_P2_PIN_SELECT);
      io_write8(gpio_data.cdr, d_tmp);
    }
    break;
  }

  vt_config_mode_exit();
}

uint16_t vt_gpio_get_baddr() {
  vt_config_mode();
  vt_config_mode_gpio();

  io_write8(gpio_data.cir, VT_LDN_GPIO_BADDR_B1);
  uint8_t b1 = io_read8(gpio_data.cdr);

  io_write8(gpio_data.cir, VT_LDN_GPIO_BADDR_B2);
  uint8_t b2 = io_read8(gpio_data.cdr);

  vt_config_mode_exit();

  return ((b1 << 8) | b2);
}

//Activate GPIO
void vt_gpio_activate() {
  vt_config_mode();
  vt_config_mode_gpio();

  io_write8(gpio_data.cir, VT_LDN_GPIO_ACTIVATE);
  io_write8(gpio_data.cdr, 0x01);

  vt_config_mode_exit();
}

int vt_init(uint8_t ports, uint16_t cir, uint16_t cdr) {
  gpio_data.cir = cir;
  gpio_data.cdr = cdr;
  uint16_t id_rev = vt_get_id_rev();
  gpio_data.dev_id  = ((id_rev & 0xFF00) >> 8);
  gpio_data.dev_rev = (id_rev & 0x00FF);

  if (gpio_data.dev_id != 0x3C) {
    return VT_INIT_NOT_FOUND;
  }

  bool some_one = false;

  if (ports & VT_CONFIG_PORT_1) {
    vt_port_as_gpio(VT_CONFIG_PORT_1);
    some_one = true;
  }

  if (ports & VT_CONFIG_PORT_3_6) {
    vt_port_as_gpio(VT_CONFIG_PORT_3_6);
    some_one = true;
  }

  if (!some_one) {
    return VT_INIT_NO_PORT;
  }

  vt_gpio_activate();
  gpio_data.baddr = vt_gpio_get_baddr();

  return VT_INIT_OK;
}

uint8_t vt_get_dev_id() {
  return gpio_data.dev_id;
}

uint8_t vt_get_dev_rev() {
  return gpio_data.dev_rev;
}

uint16_t vt_get_baddr() {
  return gpio_data.baddr;
}

void vt_port_mode(uint8_t port, uint8_t mode) {
  vt_config_mode();
  vt_config_mode_gpio();

  io_write8(gpio_data.cir, VT_LDN_GPIO_PORT_SELECT);
  io_write8(gpio_data.cdr, port);

  io_write8(gpio_data.cir, VT_LDN_GPIO_MODE_CONFIG);
  io_write8(gpio_data.cdr, mode);

  vt_config_mode_exit();
}

void vt_port_polarity(uint8_t port, uint8_t polarity) {
  vt_config_mode();
  vt_config_mode_gpio();

  io_write8(gpio_data.cir, VT_LDN_GPIO_PORT_SELECT);
  io_write8(gpio_data.cdr, port);

  io_write8(gpio_data.cir, VT_LDN_GPIO_POLAR_CONFIG);
  io_write8(gpio_data.cdr, polarity);

  vt_config_mode_exit();
}

void vt_port_write(uint8_t port, uint8_t data) {
  io_write8(gpio_data.baddr + port, data);
}

uint8_t vt_port_read(uint8_t port) {
  return io_read8(gpio_data.baddr + port);
}

void vt_pin_mode(uint8_t port, uint8_t pin, uint8_t mode) {
  vt_config_mode();
  vt_config_mode_gpio();

  io_write8(gpio_data.cir, VT_LDN_GPIO_PORT_SELECT);
  io_write8(gpio_data.cdr, port);

  io_write8(gpio_data.cir, VT_LDN_GPIO_MODE_CONFIG);
  uint8_t m_data = io_read8(gpio_data.cdr);

  if (mode == VT_PIN_OUTPUT) {
    m_data |= pin;
  } else {
    m_data &= ~(pin);
  }

  io_write8(gpio_data.cir, VT_LDN_GPIO_MODE_CONFIG);
  io_write8(gpio_data.cdr, m_data);

  vt_config_mode_exit();
}

void vt_pin_polarity(uint8_t port, uint8_t pin, uint8_t polarity) {
  vt_config_mode();
  vt_config_mode_gpio();

  io_write8(gpio_data.cir, VT_LDN_GPIO_PORT_SELECT);
  io_write8(gpio_data.cdr, port);

  io_write8(gpio_data.cir, VT_LDN_GPIO_POLAR_CONFIG);
  uint8_t p_data = io_read8(gpio_data.cdr);

  if (polarity == VT_PIN_PL_INVERSE) {
    p_data |= pin;
  } else {
    p_data &= ~(pin);
  }

  io_write8(gpio_data.cir, VT_LDN_GPIO_POLAR_CONFIG);
  io_write8(gpio_data.cdr, p_data);

  vt_config_mode_exit();
}

void vt_pin_set(uint8_t port, uint8_t pin, uint8_t value) {
  uint8_t data = vt_port_read(port);

  if (value) {
    data |= pin;
  } else {
    data &= ~(pin);
  }

  vt_port_write(port, data);
}

uint8_t vt_pin_get(uint8_t port, uint8_t pin) {
  uint8_t data = vt_port_read(port);

  return (data & pin);
}