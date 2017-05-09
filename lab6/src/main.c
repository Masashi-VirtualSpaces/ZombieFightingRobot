/*
Main program for running lab 4
Masashi "Overly Bossy" Schafer
Ross "meme master" Hartley
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
#include<audio_player.h>



//#define NUM_THREADS 1
//#define BROADCASTIP "10.122.60.41"

//Function delcarations for threads
void *broadcast(void *arg);
void *proximity(void *arg);
void *listen(void *arg);
void *songPlayer(void *arg);
void *motorController(void *arg);
void *timeTracker(void *arg);

//Global variable declarations.
double distance = 0;
const char* receivedMessage = "go";
const char* stop = "stop";
//bool DetectedObj = false;
int DetectedObj = 0;
bool PlaySong = false;
bool timeReset = false;
int timePassed = 0;

int main(){
  wiringPiSetup();

  //Set up motor PWM's initialize them to 0 with range 0-100%
  pinMode(OUT_MT_DIR_LEFT,OUTPUT);
  pinMode(OUT_MT_DIR_RIGHT,OUTPUT);
  pinMode(PWM_LEFT,OUTPUT);
  pinMode(PWM_RIGHT,OUTPUT);
  softPwmCreate(PWM_LEFT,0,100);
  softPwmCreate(PWM_RIGHT,0,100);
  digitalWrite(OUT_MT_DIR_RIGHT,0);
  digitalWrite(OUT_MT_DIR_LEFT,0);

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


//This parts activates the threads.
int rc1;
int rc2;
//int rc3;
//int rc4;
int rc5;
int rc6;
pthread_t thread1;
pthread_t thread2;
//pthread_t thread3;
//pthread_t thread4;
pthread_t thread5;
pthread_t thread6;
rc1 = pthread_create(&thread1,NULL,broadcast,(void *)NULL);
rc2 = pthread_create(&thread2,NULL,proximity,(void *)NULL);
//rc3 = pthread_create(&thread3,NULL,listen,(void *)NULL);
//rc4 = pthread_create(&thread4,NULL,songPlayer,(void *)NULL);
rc5 = pthread_create(&thread5,NULL,motorController,(void *)NULL);
rc6 = pthread_create(&thread6,NULL,timeTracker,(void *)NULL);
//Test for errors in thread initialization.
/*
if(rc1||rc2||rc3||rc4||rc5||rc6){
  printf("Error in initializing threads!\n");
  printf("broadcast: %d\nproximity: %d\nlisten: %d\nsongPlayer: %d\nmotorController: %d\ntimetracker: %d\n",rc1,rc2,rc3,rc4,rc5,rc6);
  exit(-1);
}
*/
//Main loop for running robot.
int done = 1;
while(done != 0){
  delay(2000);
  printf("Distance: %f\n",distance );
  done = strcmp(receivedMessage,stop);
}

  softPwmWrite(PWM_RIGHT,0);
  softPwmWrite(PWM_LEFT,0);

  delay(200);
  printf("Made it past switch\n");
  fflush(stdout);
  return 0;
}


/*-----------------------------------------------------------------------------
Function for broadcasting messages.
-----------------------------------------------------------------------------*/
void *broadcast(void *arg){
  printf("Now broadcasting!\n");
  char *IP = "10.122.60.41";
  char *myMessage = "Bonjour de Masashi et Ross!";
  while(1){
      delay(20000);
      printf("about to send message\n");
      broadcast_msg(myMessage,IP);
      printf("Sent message\n");
  }
}

/*-----------------------------------------------------------------------------
//Function for checking distance in front of robot.
-----------------------------------------------------------------------------*/
void *proximity(void *arg){
  printf("Now measuring distance!\n");
  init_prox();
  //double distance = 0;
  while(1)
  {
    delay(100);
    distance = getCmDistance();
    if(distance < 75 && distance > 0){
      DetectedObj = 1; //Detected an object in front of it.
      //DetectedObj = true;
      //PlaySong = true;
    }
    //printf("Current distance: %f\n",distance);
  }
  delay(2000);
}
/*-----------------------------------------------------------------------------
Function for listening for messages.
-----------------------------------------------------------------------------*/
void *listen(void *arg){
  printf("Now listening!\n");
  while(1)
  {
    receivedMessage = getUDPmessage();
    printf("message: %s\n",receivedMessage);
    delay(200);
  }
}


/*-----------------------------------------------------------------------------
Function for controlling the motors.
-----------------------------------------------------------------------------*/
void *motorController(void *arg){
  printf("Motor controller initialized!\n");
  while(1){
    switch(DetectedObj){
      case 0:
        digitalWrite(OUT_MT_DIR_RIGHT,0);
        digitalWrite(OUT_MT_DIR_LEFT,0);
        //printf("motors running forward.\n");
        softPwmWrite(PWM_RIGHT,55);
        softPwmWrite(PWM_LEFT,55);
        break;
      case 1:
      if(distance>68){
        digitalWrite(OUT_MT_DIR_RIGHT,0);
        digitalWrite(OUT_MT_DIR_LEFT,0);
        softPwmWrite(PWM_RIGHT,55);
        softPwmWrite(PWM_LEFT,55);
      }
      else if(distance < 52){
        digitalWrite(OUT_MT_DIR_RIGHT,1);
        digitalWrite(OUT_MT_DIR_LEFT,1);
        softPwmWrite(PWM_RIGHT,55);
        softPwmWrite(PWM_LEFT,55);
      }
      else{
        softPwmWrite(PWM_RIGHT,0);
        softPwmWrite(PWM_LEFT,0);
      }
      delay(250);
    }
    /*
    if(DetectedObj){
      softPwmWrite(PWM_RIGHT,0);
      softPwmWrite(PWM_LEFT,0);
      printf("motors stopped\n");
      delay(1000);
      printf("turning robot 90 degrees.\n");
      digitalWrite(OUT_MT_DIR_RIGHT,0);
      digitalWrite(OUT_MT_DIR_LEFT,1);
      softPwmWrite(PWM_RIGHT,55);
      softPwmWrite(PWM_LEFT,55);
      delay(500);
      softPwmWrite(PWM_RIGHT,0);
      softPwmWrite(PWM_LEFT,0);
      DetectedObj = false;
      timeReset = true;
    }
    else if(timePassed > 2000){
      softPwmWrite(PWM_RIGHT,0);
      softPwmWrite(PWM_LEFT,0);
      delay(2000);
      digitalWrite(OUT_MT_DIR_RIGHT,0);
      digitalWrite(OUT_MT_DIR_LEFT,1);
      softPwmWrite(PWM_RIGHT,50);
      softPwmWrite(PWM_LEFT,50);
      delay(1200);
      timeReset = true;
      //receivedMessage = "stop";
    }
    else{
      digitalWrite(OUT_MT_DIR_RIGHT,0);
      digitalWrite(OUT_MT_DIR_LEFT,0);
        //printf("motors running forward.\n");
      softPwmWrite(PWM_RIGHT,55);
      softPwmWrite(PWM_LEFT,55);
    }
    */
    delay(300);
  }
}
/*-----------------------------------------------------------------------------
Function for playing a song.
-----------------------------------------------------------------------------*/
void *songPlayer(void *arg){
  printf("songPlayer read!\n");
  init_player();
  while(1){
    if(PlaySong){
      printf("Playing Song!!!\n");
      play_file("the_cranberries_zombie_mono_short.mp3",40);
      //stopAudio();
      //delay(1000);
      PlaySong = false;
      printf("Play song reset\n");
    }
    delay(300);
  }
}

void *timeTracker(void *arg){
  while(1){
    if(timeReset){
      timePassed = 0;
      timeReset = false;
    }
    else if(!DetectedObj){
      delay(20);
      timePassed+=20;
    }
  }
}
