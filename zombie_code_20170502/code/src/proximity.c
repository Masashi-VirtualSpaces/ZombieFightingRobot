#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h> 
#include <sys/time.h>
#include <pthread.h>

#include "utils.h"
#include "pinout.h"
#include "proximity.h"

struct timeval rise_time, fall_time;
pthread_t setPulseThr;

// Internal Function declarations
void riseProxiInterrupt (void);
void fallProxiInterrupt (void);
void* setPulse (void* arg);
double getTimeDiff (void);

void riseProxiInterrupt (void)
{
  gettimeofday (&rise_time, NULL);
}

void fallProxiInterrupt (void)
{
  gettimeofday (&fall_time, NULL);
}

// setPulse: proximity Thread, runs continuously to set pulse
void* setPulse (void* arg)
{
  while (1)
  {
    digitalWrite(OUT_US_TRIG, HIGH);
    usleep(15);
    digitalWrite(OUT_US_TRIG, LOW);
    delay(40);
  }
  return (void*) 0;
}

void stopPulse (void)
{
  pthread_cancel(setPulseThr);
  digitalWrite(OUT_US_TRIG, LOW);
}


double getTimeDiff (void)
{
  return time_diff(fall_time, rise_time);
}

double getCmDistance (void)
{
  double diff = getTimeDiff();
  if(diff > 0 && diff < 11000)
  {
    return diff*.017 - 3.4;
  }
  return -1;
}

double getInDistance (void)
{
  double diff = getTimeDiff();
  if(diff > 0 && diff < 11000)
  {
    return diff*.0067 - 1.3;
  }
  return -1;
}
// init_prox: initializes all proximity sensor pins (GPIO allocation and initial values of outputs) 
void init_prox(void) 
{
  pinMode(OUT_US_TRIG, OUTPUT) ;
  pinMode(IN_US_ECHO_1, INPUT);
  pinMode(IN_US_ECHO_2, INPUT);
  
  wiringPiISR(IN_US_ECHO_1, INT_EDGE_RISING, &riseProxiInterrupt);
  wiringPiISR(IN_US_ECHO_2, INT_EDGE_FALLING, &fallProxiInterrupt);
  
  int ret = pthread_create( &(setPulseThr), NULL, setPulse, NULL);
  if( ret )
  {
    fprintf(stderr,"Error creating setPulseThr - pthread_create() return code: %d\n",ret);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  printf("Started proximity sensor\n"); fflush(stdout);
  return;
}

