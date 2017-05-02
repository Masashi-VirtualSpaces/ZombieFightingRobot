#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "motors.h"
#include "pinout.h"

// data structure of a single motor control
typedef struct {
  int driver_pin;         // PWM pin if HW-driven
  int fb_pin;             // Feedback pin (encoder)
  int dir_pin;            // Direction pin
  int stopFlag;           // Stop Flag
  int pulseCounter;       // current pulse count (encoder). Can be negative.
  int direction;          // direction value (0: backward, 1: forward)
  int dutyCycle;          // duty cycle, in percent (0 - 100)
  int targetCount;        // target count (desired final count). Can be negative
} _t_motor;

_t_motor a_motors[2];  // motor control instances
static int init_motors_done;

// ISR functions for the encoder counters
// incrementing when direction is '1', decreasing when direction is '0'
void myInterrupt0 (void)
{
  if(a_motors[0].direction)
    ++a_motors[0].pulseCounter;
  else
    --a_motors[0].pulseCounter;
}
void myInterrupt1 (void)
{
  if(a_motors[1].direction)
    ++a_motors[1].pulseCounter;
  else
    --a_motors[1].pulseCounter;
}

// motorDrive: function invoked by motor Threads, runs continuously
void* motorDrive (void* num)
{
  int motor_num = *(int*)num;
  a_motors[motor_num].stopFlag = 0;

  while (!a_motors[motor_num].stopFlag)
  {
    
    /******************************************/
    // TODO: Implement your motor control here!
    /******************************************/

    delay(50);
  }
  return (void*) 0;
}

// init_motors: initializes all motor pins (GPIO allocation and initial values of outputs) 
// and initialize the elements of all motor control data structure
int init_motors(void) 
{
  int j;
  // iterated for each motors
  for(j=0; j<2; j++)
  {
    a_motors[j].pulseCounter = 0;
    a_motors[j].stopFlag = 1;
    a_motors[j].targetCount = 0;
    switch(j)
    {
      case LEFT_MOTOR:
        a_motors[j].driver_pin = PWM_LEFT;
        a_motors[j].fb_pin = IN_MT_LEFT;
        a_motors[j].dir_pin = OUT_MT_DIR_LEFT;
        break;
      case RIGHT_MOTOR:
        a_motors[j].driver_pin = PWM_RIGHT;
        a_motors[j].fb_pin = IN_MT_RIGHT;
        a_motors[j].dir_pin = OUT_MT_DIR_RIGHT;
        break;
    }
    softPwmCreate(a_motors[j].driver_pin,0, 100);
    softPwmWrite(a_motors[j].driver_pin, 0);
    pinMode(a_motors[j].fb_pin, INPUT);
    pinMode(a_motors[j].dir_pin, OUTPUT);
    digitalWrite(a_motors[j].dir_pin, 1);
  }
  init_motors_done = 1;
  return 0;
}

// stopMotor: stop a specific motor and its respective thread
void stopMotor(int motor_num)
{
    if(init_motors_done)
    {
        a_motors[motor_num].stopFlag = 1;
        setDutyCycle(motor_num, 0);
    }
}

// stopAllMotors: stop all motors and the motor threads 
void stopAllMotors(void)
{
    int i;
    for(i =0; i<2; i++)
        stopMotor(i);
}

// start_ISRs: start all ISR routines (wheel encoder counters)
void start_ISRs(void)
{
    if(init_motors_done)
    {
        wiringPiISR(a_motors[0].fb_pin, INT_EDGE_FALLING, &myInterrupt0);
        wiringPiISR(a_motors[1].fb_pin, INT_EDGE_FALLING, &myInterrupt1);
    }
    else
    {
        // Handles an error from the motor initalization
        fprintf(stderr, "motor initialization required before start_ISRs\n");
        fflush(stderr);
    }
}

// getCount: accessor funtion of a motor pulse counter
int getCount(int motor_num)
{
    if(init_motors_done)
    {
        return a_motors[motor_num].pulseCounter;
    }
    else
    {
        // Handles an error from the motor initalization
        fprintf(stderr, "motor initialization required before getCount\n");
    }
    return -1;
}

// getDirection: accessor funtion of a motor direction
int getDirection(int motor_num)
{
    if(init_motors_done)
    {
        return a_motors[motor_num].direction;
    }
    else
    {
        // Handles an error from the motor initalization
        fprintf(stderr, "motor initialization required before getDirection\n");
        fflush(stderr);
    }
    return -1;
}

// setDutyCycle: sets a motor PWM duty-cycle and its direction value
void setDutyCycle(int motor_num, int dutyCycle)
{
    if(init_motors_done)
    {
        a_motors[motor_num].dutyCycle = dutyCycle <= 100 ? dutyCycle : 100;
        softPwmWrite(a_motors[motor_num].driver_pin, a_motors[motor_num].dutyCycle);
        delay(10);
    }
    else
    {
        // Handles an error from the motor initalization
        fprintf(stderr, "motor initialization required before setDutyCycle\n");
        fflush(stderr);
    }
}

// setDirection: set the direction value (0: backward, 1: forward) of a motor
void setDirection(int motor_num, int direction)
{
    if(init_motors_done)
    {
        a_motors[motor_num].direction = direction;
        digitalWrite(a_motors[motor_num].dir_pin, direction);
    }
    else
    {
        // Handles an error from the motor initalization
        fprintf(stderr, "motor initialization required before setDutyCycle\n");
        fflush(stderr);
    }
}
