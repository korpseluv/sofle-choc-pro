// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#define USB_VBUS_PIN GP13

/* RP2040- and hardware-specific config */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U
#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64

/* OLED on I2C1 - pins taken from Keebart's Corne Choc Pro, unverified on the Sofle */
#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP6
#define I2C1_SCL_PIN GP7
#define OLED_DISPLAY_128X64
#define OLED_UPDATE_INTERVAL 50
