
#ifndef PINOUT_H
#define PINOUT_H

// GPIO Input Pins
#define IN_SW_UTIL_1       1  // pin 12, GPIO_1
#define IN_SW_UTIL_2       6  // pin 22, GPIO_6
#define IN_SW_EMAIL       25  // pin 37, GPIO_25
#define IN_IR_RECV         5  // pin 18, GPIO_5
#define IN_US_ECHO_1       2  // pin 13, GPIO_2
#define IN_US_ECHO_2       3  // pin 15, GPIO_3
#define IN_MT_LEFT        24  // pin 35, GPIO_24
#define IN_MT_RIGHT       28  // pin 38, GPIO_28

// GPIO Output Pins
#define OUT_IR_EMIT        4 // pin 16, GPIO_4
#define OUT_US_TRIG        0 // pin 11, GPIO_0
#define OUT_MT_DIR_LEFT   27 // pin 36, GPIO_27
#define OUT_MT_DIR_RIGHT  22 // pin 31, GPIO_22
#define OUT_AUDIO_EN      21 // pin 29, GPIO_21

// PWM Output Pins
#define PWM_LEFT          26 // pin 32, PWM_0
#define PWM_RIGHT         23 // pin 33, PWM_1

#endif
