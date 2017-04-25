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

int main(){
  wiringPiSetupGpio();

  //Declare global variable for communication between threads.
  bool run = true;
  bool DetectedObj = false;
  bool PlaySong = false;
  double CurrentDistance = 0;

  //Wait for switch to be hit in order to start program.
  //pinMode()
  bool switchWait = true;

  cout<<"about to go into wait mode. Activate"<<endl;

  while(switchWait){
    if(digitalRead(IN_SW_UTIL_1) == 0){
      switchWait = false;
    }
  }

  cout<<"Made it past switch"<<endl;

}
