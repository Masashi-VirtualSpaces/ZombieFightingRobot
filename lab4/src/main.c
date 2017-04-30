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



#define NUM_THREADS 1
//#define BROADCASTIP "10.122.60.41"

//Function delcarations for threads
void *broadcast(void *arg);
void *proximity(void *arg);
void *listen(void *arg);
void *songPlayer(void *arg);
//Global variable declarations.
double distance = 0;
const char* receivedMessage = "go";
const char* stop = "stop";
bool DetectedObj = false;
bool PlaySong = false;

int main(){
  wiringPiSetup();

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


//This parts activates the threads.
int rc1;
int rc2;
int rc3;
int rc4;
pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;
rc1 = pthread_create(&thread1,NULL,broadcast,(void *)NULL);
rc2 = pthread_create(&thread2,NULL,proximity,(void *)NULL);
rc3 = pthread_create(&thread3,NULL,listen,(void *)NULL);
rc4 = pthread_create(&thread4,NULL,songPlayer,(void *)NULL);

int done = 1;

while(done != 0){
  delay(2000);
  printf("Distnace: %f\n",distance );
  done = strcmp(receivedMessage,stop);
}
/*
pthread_t threads[NUM_THREADS];
int rc;
long t;
for(t=0; t<NUM_THREADS; t++){
   printf("In main: creating thread %ld\n", t);
   rc = pthread_create(&threads[t], NULL, broadcast, (void *)t);
   if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
*/


  delay(200);
  printf("Made it past switch\n");
  fflush(stdout);

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
    delay(200);
    distance = getCmDistance();
    if(distance < 18 && distance > 0){
      DetectedObj = true;
      PlaySong = true;
    }
    //printf("Current distance: %f\n",distance);
  }
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

/*-----------------------------------------------------------------------------
Function for playing a song.
-----------------------------------------------------------------------------*/
void *songPlayer(void *arg){
  printf("songPlayer read!\n");
  while(1){
    if(PlaySong){
      printf("Playing Song!!!\n");
      delay(1000);
      PlaySong = false;
      printf("Play song reset\n");
    }
    delay(200);
  }
}
