/**
 * Marlin 3D Printer Firmware
 *
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 * Copyright (c) 2016 Bob Cousins bobcousins42@googlemail.com
 * Copyright (c) 2015-2016 Nico Tonnhofer wurstnase.reprap@gmail.com
 * Copyright (c) 2017 Victor Perez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#define CPU_32_BIT

#include "../../core/macros.h"
#include "../shared/Marduino.h"
#include "../shared/math_32bit.h"
#include "../shared/HAL_SPI.h"
#include "fastio.h"
#include "watchdog.h"
#include "MarlinSerial.h"

#include "../../inc/MarlinConfigPre.h"

#include <stdint.h>

#ifdef USBCON
  #include <USBSerial.h>
#endif
// #if defined (USE_GD32)
  // #include "../../GD32_USBSerial.h"
// #endif
// ------------------------
// Defines
// ------------------------
#define _MSERIAL(X) MSerial##X
#define MSERIAL(X) _MSERIAL(X)

#if SERIAL_PORT == -1
  #define MYSERIAL0 SerialUSB
#elif WITHIN(SERIAL_PORT, 1, 6)
  #define MYSERIAL0 MSERIAL(SERIAL_PORT)
#else
  #error "SERIAL_PORT must be -1 or from 1 to 6. Please update your configuration."
#endif

#ifdef SERIAL_PORT_2
  #if SERIAL_PORT_2 == -1
    #define MYSERIAL1 SerialUSB
  #elif WITHIN(SERIAL_PORT_2, 1, 6)
    #define MYSERIAL1 MSERIAL(SERIAL_PORT_2)
  #else
    #error "SERIAL_PORT_2 must be -1 or from 1 to 6. Please update your configuration."
  #endif
#endif

#ifdef SERIAL_PORT_3
  #if SERIAL_PORT_3 == -1
    #define MYSERIAL2 SerialUSB
  #elif WITHIN(SERIAL_PORT_3, 1, 6)
    #define MYSERIAL2 MSERIAL(SERIAL_PORT_3)
  #else
    #error "SERIAL_PORT_3 must be -1 or from 1 to 6. Please update your configuration."
  #endif
#endif

#ifdef LCD_SERIAL_PORT
  #if LCD_SERIAL_PORT == -1
    #define LCD_SERIAL SerialUSB
  #elif WITHIN(LCD_SERIAL_PORT, 1, 6)
    #define LCD_SERIAL MSERIAL(LCD_SERIAL_PORT)
  #else
    #error "LCD_SERIAL_PORT must be -1 or from 1 to 6. Please update your configuration."
  #endif
  #if HAS_DGUS_LCD
    #define SERIAL_GET_TX_BUFFER_FREE() LCD_SERIAL.availableForWrite()
  #endif
#endif

/**
 * TODO: review this to return 1 for pins that are not analog input
 */
#ifndef analogInputToDigitalPin
  #define analogInputToDigitalPin(p) (p)
#endif

#define CRITICAL_SECTION_START()  uint32_t primask = __get_PRIMASK(); __disable_irq()
#define CRITICAL_SECTION_END()    if (!primask) __enable_irq()
#define ISRS_ENABLED() (!__get_PRIMASK())
#define ENABLE_ISRS()  __enable_irq()
#define DISABLE_ISRS() __disable_irq()
#define cli() __disable_irq()
#define sei() __enable_irq()

// On AVR this is in math.h?
#define square(x) ((x)*(x))

#ifndef strncpy_P
  #define strncpy_P(dest, src, num) strncpy((dest), (src), (num))
#endif

// Fix bug in pgm_read_ptr
#undef pgm_read_ptr
#define pgm_read_ptr(addr) (*(addr))

// ------------------------
// Types
// ------------------------

typedef int16_t pin_t;

#define HAL_SERVO_LIB libServo

// ------------------------
// Public Variables
// ------------------------

// result of last ADC conversion
extern uint16_t HAL_adc_result;

// ------------------------
// Public functions
// ------------------------

// Memory related
#define __bss_end __bss_end__

// Enable hooks into  setup for HAL
void HAL_init();

// Clear reset reason
void HAL_clear_reset_source();

// Reset reason
uint8_t HAL_get_reset_source();

inline void HAL_reboot() {}  // reboot the board or restart the bootloader

void _delay_ms(const int delay);

extern "C" char* _sbrk(int incr);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static inline int freeMemory() {
  volatile char top;
  return &top - reinterpret_cast<char*>(_sbrk(0));
}

#pragma GCC diagnostic pop

//
// ADC
//
#if 0//ENABLED(NEW_BOARD)
  typedef struct {
    PinName pin;
    ADC_TypeDef * ADCx;
    uint32_t channel;
  } my_PinMap_ADC;

  const my_PinMap_ADC ADC_Channel[] = {
    {PF_5,  ADC3, ADC_CHANNEL_15},
    {PF_6,  ADC3, ADC_CHANNEL_4},
    {PF_7,  ADC3, ADC_CHANNEL_5},
    {PF_8,  ADC3, ADC_CHANNEL_6},
    {PF_9,  ADC3, ADC_CHANNEL_7},
    {NC,    NP,    0}
  };
  const PinMap PinMap_ADC_1[] = {
    {PF_5,  ADC3,  STM_PIN_DATA_EXT(STM_MODE_ANALOG, GPIO_NOPULL, 0, 15, 0)}, // ADC3_IN15
    {PF_6,  ADC3,  STM_PIN_DATA_EXT(STM_MODE_ANALOG, GPIO_NOPULL, 0,  4, 0)}, // ADC3_IN4
    {PF_7,  ADC3,  STM_PIN_DATA_EXT(STM_MODE_ANALOG, GPIO_NOPULL, 0,  5, 0)}, // ADC3_IN5
    {PF_8,  ADC3,  STM_PIN_DATA_EXT(STM_MODE_ANALOG, GPIO_NOPULL, 0,  6, 0)}, // ADC3_IN6
    {PF_9,  ADC3,  STM_PIN_DATA_EXT(STM_MODE_ANALOG, GPIO_NOPULL, 0,  7, 0)}, // ADC3_IN7
    {NC,    NP,    0}
  };
  void MY_ADC_Configuration(ADC_TypeDef * adc_num);
  void MY_ADC3_Init(const uint8_t adc_pin);
  void MY_ADC_Init_P(const uint8_t adc_pin);
  uint16_t Get_Adc(const uint8_t adc_pin);

  #define HAL_ANALOG_SELECT(pin) MY_ADC3_Init(pin)
  // #define HAL_ANALOG_SELECT_P(pin) MY_ADC_Init_P(pin)
#else
  #define HAL_ANALOG_SELECT(pin) pinMode(pin, INPUT)
#endif

#define HAL_ADC_VREF        3.3
#define HAL_ADC_RESOLUTION  ADC_RESOLUTION
#define HAL_START_ADC(pin)  HAL_adc_start_conversion(pin)
#ifndef USE_GD32
  #define HAL_READ_ADC()      (HAL_adc_result) //stm32.  adc cfg correct.
#else
  #define HAL_READ_ADC()      (HAL_adc_result>>2) //in GD32 ADC cfg 10, but read still 12.  
#endif
#define HAL_ADC_READY()     true

inline void HAL_adc_init() { analogReadResolution(HAL_ADC_RESOLUTION);/*TERN_(NEW_BOARD, MY_ADC_Configuration(ADC3);)*/}
void HAL_adc_start_conversion(const uint8_t adc_pin);

uint16_t HAL_adc_get_result();

#define GET_PIN_MAP_PIN(index) index
#define GET_PIN_MAP_INDEX(pin) pin
#define PARSED_PIN_INDEX(code, dval) parser.intval(code, dval)

#ifdef STM32F1xx
  #define JTAG_DISABLE() AFIO_DBGAFR_CONFIG(AFIO_MAPR_SWJ_CFG_JTAGDISABLE)
  #define JTAGSWD_DISABLE() AFIO_DBGAFR_CONFIG(AFIO_MAPR_SWJ_CFG_DISABLE)
#endif

#define PLATFORM_M997_SUPPORT
void flashFirmware(const int16_t);

// Maple Compatibility
typedef void (*systickCallback_t)(void);
void systick_attach_callback(systickCallback_t cb);
void HAL_SYSTICK_Callback();

/**
 * set_pwm_frequency
 *  Set the frequency of the timer corresponding to the provided pin
 *  All Timer PWM pins run at the same frequency
 */
void set_pwm_frequency(const pin_t pin, int f_desired);

/**
 * set_pwm_duty
 *  Set the PWM duty cycle of the provided pin to the provided value
 *  Optionally allows inverting the duty cycle [default = false]
 *  Optionally allows changing the maximum size of the provided value to enable finer PWM duty control [default = 255]
 */
void set_pwm_duty(const pin_t pin, const uint16_t v, const uint16_t v_size=255, const bool invert=false);
