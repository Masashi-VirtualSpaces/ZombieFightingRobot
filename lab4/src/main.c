/*
Main program for running lab 4
Masashi Schafer
Ross the meme master Hartley
04/25/2017
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include<stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h>
#include <math.h>
#include<pinout.h>
#include<proximity.h>
#include<utils.h>
#include<web.h>
#include<softPwm.h>

int main(){
  wiringPiSetupGpio();
  pinMode(12,INPUT);
  //Declare global variable for communication between threads.
  bool run = true;
  bool DetectedObj = false;
  bool PlaySong = false;
  double CurrentDistance = 0;

  //Set up motor PWM's initialize them to 0 with range 0-100%
  softPwmCreate(PWM_LEFT,0,100);
  softPwmCreate(PWM_RIGHT,0,100);

  //Wait for switch to be hit in order to start program.
  //pinMode()
  bool switchWait = true;
  printf("about to go into wait mode. Activate\n");

  while(switchWait){
    if(digitalRead(IN_SW_UTIL_1)){
      switchWait = false;
      softPwmWrite(PWM_LEFT,0);
      softPwmWrite(PWM_RIGHT,0);
    }
  }

  printf("Made it past switch\n");

}