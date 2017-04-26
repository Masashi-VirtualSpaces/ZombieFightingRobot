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
#include <stdbool.h>
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
#include<pthread.h>


//#define BROADCASTIP "10.122.60.41"

void broadcast();

int main(){
  wiringPiSetup();

  //Declare global variable for communication between threads.
  bool run = true;
  bool DetectedObj = false;
  bool PlaySong = false;
  double CurrentDistance = 0;

  //Set up motor PWM's initialize them to 0 with range 0-100%
  pinMode(PWM_LEFT,OUTPUT);
  pinMode(PWM_RIGHT,OUTPUT);
  softPwmCreate(PWM_LEFT,0,100);
  softPwmCreate(PWM_RIGHT,0,100);

  //Wait for switch to be hit in order to start program.
  //pinMode()
  bool switchWait = true;
  pinMode(IN_SW_UTIL_1,INPUT);
  printf("about to go into wait mode.\n");
  printf("digitalRead(IN_SW_UTIL_1):%d\n", digitalRead(IN_SW_UTIL_1));

  while(switchWait){
    if(digitalRead(IN_SW_UTIL_1)==0){
      switchWait = false;
      //break;
    }
    delay(100);
  }

  //This section activates threads.
  int myBroadcast;
  myBroadcast = pthread_create(1,NULL,broadcast,NULL);
  //broadcast(BROADCASTIP);
  delay(200);
  printf("Made it past switch\n");
  fflush(stdout);

}

void broadcast(){
  char *IP = "10.122.60.41";
  char *myMessage = "Bonjour de Masashi et Ross!";
  //whiel(1){
  //    delay(20000);
      broadcast_msg(myMessage,IP);
      printf("Sent message");
//  }
}
/*
void proximity(){

}
*/
