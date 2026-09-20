#pragma once
#define TAPPING_TERM 175
#define TAPPING_TERM_PER_KEY

#define EE_HANDS
#define DEBUG
#define SPLIT_USB_DETECT
#define SPLIT_TRANSPORT_SERIAL
#define MASTER_LEFT
#define USE_SERIAL_PD2
#define USB_SUSPEND_WAKEUP_DELAY 0

// Both halves have sensors; combine their reports. Use a conservative
// throttle because the RP2040 vendor serial link is easily saturated when
// both sides are polled.
#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_COMBINED

// No throttle: both sensors are polled as fast as the main loop allows.
// The default QMK minimum interval is 1 ms.

// Give the slave time to respond during pointing-device transactions.
// Keep it short enough that an RPC failure doesn't freeze the main loop.
#define SERIAL_USART_TIMEOUT 50

// Allow more failed split-transaction attempts before declaring the slave dead.
#define SPLIT_MAX_CONNECTION_ERRORS 20

#define SPI_DRIVER SPID0

#define SPI_SCK_PIN GP2
#define SPI_MOSI_PIN GP3
#define SPI_MISO_PIN GP4
#define PMW33XX_CS_PIN GP5

#define PMW33XX_MOTION_PIN GP6

// PMW33xx sensor configuration

// Default CPI / sensitivity for both sensors
#define PMW33XX_CPI 7200

// // Chip select on Pro Micro pin "9" (PB5)
// #define PMW33XX_CS_PIN B5
//
// // Optional reset pin on Pro Micro pin "8" (PB4)
// #define PMW33XX_RESET_PIN B4
//
// // Motion interrupt on Pro Micro pin "3" (PD1)
// #define PMW33XX_MOTION_PIN D0
//
// // Hardware SPI pins on ATmega32U4
// #define SPI_SCK_PIN B1
// #define SPI_MOSI_PIN B2
// #define SPI_MISO_PIN B3
//
// // Conservative SPI speed for bring-up/debugging
// #define PMW33XX_SPI_DIVISOR 64
//
// // Debug
// #define POINTING_DEVICE_DEBUG
