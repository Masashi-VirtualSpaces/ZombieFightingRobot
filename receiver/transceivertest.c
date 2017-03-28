/*
 transceivertest.c:
 
 Compile command:
    gcc -o xcvr transceivertest.c -Wall -lwiringPi

 Usage:
   sudo ./xcvr
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h> 
#include <wiringPi.h> 


void control_event(int sig);

struct timeval ir_edge_time[20];
int edge_state[20];
unsigned int edge_cnt;

double time_diff(struct timeval end , struct timeval start);

void edgeIRInterrupt (void)
{
  if (edge_cnt < 20)
  {
    gettimeofday (&ir_edge_time[edge_cnt], NULL);
	edge_state[edge_cnt++] = digitalRead(5);
  }
}

/* Returns time difference in us */
double time_diff(struct timeval end , struct timeval start)
{
  return (double)end.tv_sec*1000000 + (double)end.tv_usec - (double)start.tv_sec*1000000 - (double)start.tv_usec;
}


int main()
{
    double diff[19];
	(void)signal(SIGINT,control_event);
    (void)signal(SIGQUIT,control_event);
    (void)signal(SIGTERM,control_event);

    edge_cnt = 0;
    int i;
    for( i=0; i<20; i++){
        gettimeofday (&ir_edge_time[i], NULL);
    }
 
    wiringPiSetup();
	pinMode(5, INPUT);
	wiringPiISR(5, INT_EDGE_BOTH, &edgeIRInterrupt);

	printf("\n");
	while(1)
	{
        if(edge_cnt == 20)
        {
			for( i=0; i<19; i++){
				diff[i] = time_diff(ir_edge_time[i+1], ir_edge_time[i]);
				printf("i:%d , diff= %fms state= %d\n", i, diff[i]/1000, edge_state[i]);
			}
			printf("\n");
			/* EDIT THIS SECTION To parse diff/edge_state AND print the character*/
			
            delay(100);
			edge_cnt = 0;
        }
	}
}

void control_event(int sig)
{
  printf("\n  Exiting...");
  delay(200);
  printf("\n");
  exit(0);
}