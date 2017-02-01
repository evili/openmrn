#ifndef _HARDWARE_HXX_
#define _HARDWARE_HXX_

#include "Stm32Gpio.hxx"
#include "utils/GpioInitializer.hxx"

GPIO_PIN(SW1, GpioInputPD, C, 13);

GPIO_PIN(LED_RAW_1, LedPin, B, 0);
GPIO_PIN(LED_2, LedPin, B, 7);
GPIO_PIN(LED_3, LedPin, B, 14);

typedef LED_RAW_1_Pin BLINKER_RAW_Pin;

typedef GpioInitializer<                          //
    SW1_Pin,                                      //
    LED_RAW_1_Pin, LED_2_Pin, LED_3_Pin>
    GpioInit;

#endif // _HARDWARE_HXX_
