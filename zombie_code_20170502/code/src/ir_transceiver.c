#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h> 
#include <math.h>
#include <pthread.h>
#include <wiringPi.h> 

#include "utils.h"
#include "pinout.h"
#include "ir_transceiver.h"

#define MAX_EDGE      20
#define MAX_NUMBIT    MAX_EDGE - 1

// Internal Function declarations
void edgeIRInterrupt (void);
void* transreceiver (void* null_ptr);


struct timeval ir_edge_time[MAX_EDGE];
int edge_state[MAX_EDGE];
unsigned int edge_cnt;
char xmt_letter;
char rcv_letter;
char *xmt_bits;
pthread_t transreceiverThr;
static int transmit_mode;
int baudRate;
int numDataBits;
int setParity;
int parityType;
int payload_len; 


void edgeIRInterrupt (void)
{
    if (edge_cnt < MAX_EDGE)
    {
        gettimeofday (&ir_edge_time[edge_cnt], NULL);
        edge_state[edge_cnt++] = !digitalRead(IN_IR_RECV);
    }
}


void* transreceiver(void* null_ptr)
{
    int i,j,k;
    long diff[MAX_NUMBIT];
    const double bit_per_us = baudRate * 1e-6;
    const int bit_time_us = (int)((1.0/baudRate) * 1e6);
    const int idle_time_us = 25000;

    edge_cnt = 0;
    
    while(1)
    {
        while(transmit_mode)
        {
            edge_cnt = MAX_EDGE;
            for(i=0; i < payload_len && xmt_letter != 0 && transmit_mode; i++)
            {
                digitalWrite(OUT_IR_EMIT, xmt_bits[i]);
                usleep(bit_time_us);
            }
            digitalWrite(OUT_IR_EMIT, HIGH);
            usleep(idle_time_us);
        }

        while(!transmit_mode)
        {
            if(edge_cnt == MAX_EDGE)
            {
                int numbit[MAX_NUMBIT], start_idx = -1;
                for(k = 0; k < MAX_NUMBIT && !transmit_mode; k++)
                {
                    diff[k] = time_diff(ir_edge_time[k+1], ir_edge_time[k]);
                    numbit[k] = (int)(round(diff[k] * bit_per_us));
                    if(start_idx < 0 && numbit[k] > 20)
                    {
                        start_idx = k + 1;
                    }
                }
                
                if(numbit[start_idx] != 0 
                   && numbit[start_idx+1] < payload_len 
                   && edge_state[start_idx] == 0
                   && !transmit_mode)
                {
                    int bits[payload_len];
                    int parity_bit = 0;
                    for(k=start_idx, i=0; i < payload_len 
                        && k < MAX_NUMBIT 
                        && !transmit_mode; k++)
                    {
                        for(j=0; j < numbit[k]
                            && i < payload_len 
                            && !transmit_mode; j++, i++)
                        {
                            bits[i] = edge_state[k];
                            parity_bit ^= bits[i];
                        }
                    }
                    if(!transmit_mode && (parity_bit == parityType || !setParity))
                    {
                        char tmp_letter = 0;
                        for(i=0; i < numDataBits && !transmit_mode; i++)
                        {
                            tmp_letter |= bits[i+1] << i;
                        }
                        if(tmp_letter >= ' ' && tmp_letter <= '~' && !transmit_mode)
                        {
                            rcv_letter = tmp_letter;
                        }
                    }
                }
                if(!transmit_mode)
                    delay(25);
                edge_cnt = 0;
            }
        }
    }
    return (void*) 0;
}

int init_ir(int baud, int num_data_bits, int set_parity, int parity_type) 
{
    int i, ret;
    baudRate = baud;
    numDataBits = num_data_bits;
    setParity = set_parity;
    parityType = parity_type;
    payload_len = numDataBits + setParity + 1;
    xmt_bits = malloc(payload_len*sizeof(char));
    edge_cnt = 0;

    rcv_letter = 0;
    xmt_letter = 0;

    for(i = 0; i < MAX_EDGE; i++)
    {
        gettimeofday (&ir_edge_time[i], NULL);
    }
    pinMode(IN_IR_RECV, INPUT);
    pullUpDnControl(IN_IR_RECV, PUD_OFF);
    pinMode(OUT_IR_EMIT, OUTPUT);
    digitalWrite(OUT_IR_EMIT, HIGH);
    wiringPiISR(IN_IR_RECV, INT_EDGE_BOTH, &edgeIRInterrupt);
    setTransmitModeOff();

    ret = pthread_create( &(transreceiverThr), NULL, transreceiver, NULL);
    if( ret )
    {
        fprintf(stderr,"Error creating transreceiverThr - pthread_create() return code: %d\n",ret);
        fflush(stderr);
        return ret;
    }
    return 0;
}

void stopIr(void)
{
    pthread_cancel(transreceiverThr);
    setTransmitModeOff();
    digitalWrite(OUT_IR_EMIT, HIGH);
}

char getRcvLetter(void)
{
    return rcv_letter;
}

void setXmtLetter(char c)
{
    int parity_bit = parityType;
    int i;
    xmt_letter = c;
    for(i=0; i < 1; i++)
    {
        xmt_bits[i] = 0;
    }
    for(i=0; i < numDataBits; i++)
    {
        int mask = 1 << i;
        int bit = (xmt_letter & mask) >> i;
        parity_bit ^= bit;
        xmt_bits[i+1] = bit;
    }
    if(setParity)
    {
        xmt_bits[i+1] = parity_bit;
    }
}

void setTransmitModeOn(void)
{
    transmit_mode = 1;
}

void setTransmitModeOff(void)
{
    edge_cnt = 0;
    transmit_mode = 0;
}