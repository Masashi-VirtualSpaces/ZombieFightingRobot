/**
\file       diagnostic.c
\author     Eddy Ferre - ferree@seattleu.edu
\date       04/01/2017

\brief      Main diagnostic file.

This program sets up various sensors and actuators, then allows the user to interact from the console.

*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wiringPi.h>

#include "motors.h"
#include "proximity.h"
#include "ir_transceiver.h"
#include "audio_player.h"
#include "web.h"

#define MP3_FILE  "./res/the_cranberries_zombie_mono_short.mp3"

// Local function declaration
void control_event(int sig);
int encoderTest(void);
int timedMotorDriveTest(void);
int rampUpPwmTest(void);
void proximityTest(void);
void IrReceiverTest(void);
void IrTransmitterTest(void);
void IrTransceiverTest(void);
int audioTest(void);
void UDPBroadcastTest(void);
void UDPListennerTest(void);
void irController(void);

/**
main function - Entry point function for diagnostic

@param argc number of arguments passed
@param *argv array of string argumnents

@return number '0' exit code when no error found.
*/
int main (int argc, char *argv[])
{
    // Inform OS that control_event() function will be handling kill signals
    (void)signal(SIGINT,control_event);
    (void)signal(SIGQUIT,control_event);
    (void)signal(SIGTERM,control_event);

    // Initialize the Wiring Pi facility
    if (wiringPiSetup() != 0)
    {
        // Handles error Wiring Pi initialization
        fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
        fflush(stderr);
        return 1;
    }

    // Parse the function argument
    int test_num;
    char c;
    if(argc < 2)
    {
        printf("  Select one of the test below:\n");
        printf("    0 - Encoder Test\n");
        printf("    1 - Timed Motor Drive Test\n");
        printf("    2 - Ramp-Up PWM Test\n");
        printf("    3 - Proximity Test\n");
        printf("    4 - Infra-Red Receiver Test\n");
        printf("    5 - Infra-Red Transmitter Test\n");
        printf("    6 - Infra-Red Transceiver Test\n");
        printf("    7 - Audio Test\n");
        printf("    8 - UDP Broadcast Test\n");
        //printf("    9 - UDP Listener Test\n");
        printf("    9 - lab 5 Test\n");
        printf("  Type a test number then press ENTER... > ");
        scanf(" %c", &c);
    }
    else
    {
        c = argv[1][0];
    }

    if( c < '0' || c > '9')
    {
        // Handles argument type error
        fprintf(stderr, "Argument must be a number, received %c\n", c);
        fflush(stderr);
        return 1;
    }
    test_num = c - '0';

    // Select the test to run
    switch(test_num)
    {
    case 0:
        encoderTest();
        break;
    case 1:
        timedMotorDriveTest();
        break;
    case 2:
        rampUpPwmTest();
        break;
    case 3:
        proximityTest();
        break;
    case 4:
        IrReceiverTest();
        break;
    case 5:
        IrTransmitterTest();
        break;
    case 6:
        IrTransceiverTest();
        break;
    case 7:
        audioTest();
        break;
    case 8:
        UDPBroadcastTest();
        break;
    /*case 9:
        UDPListennerTest();
        break;*/
    case 9:
        irController();
        break;
    default:
        fprintf(stderr, "Cannot handle argument \"%s\" (=%d)\n", argv[1], test_num);
        fflush(stderr);
        return 1;
    }
    printf("Test completed\n");
    return 0;
}

/**
encoderTest function

Initialize the motor encoder ISR and monitors the encoder counts.
Displays the counter absolute values for each motor encoders

@return number '0' exit code when no error found.
*/
int encoderTest(void)
{
    int i;
    int counts[2];

    printf("Running encoder Test\n");
    // Initialize motor encoder (variables, GPIO pins)
    if (init_motors() != 0)
    {
        // Handles an error from the motor initalization
        fprintf(stderr, "Unable to initialize the motors: %s\n", strerror(errno));
        fflush(stderr);
        return 1;
    }
    // Start the Interrupt Service Routines monitoring the encoders
    start_ISRs();
    //Display initial counter values
    for(i=0; i<2; i++)
    {
        counts[i] = getCount(i);
        printf("  %s motor : %d\n", i == LEFT_MOTOR ? "left" : "right", abs(counts[i]));
    }
    // Console instruction to move the motor or wheels
    printf("Move any motors then press ENTER...");
    // Waits for the 'ENTER' key
    getchar();
    // Display the new counter values in the console
    printf("New counts:\n");
    for(i=0; i<2; i++)
    {
        counts[i] = getCount(i);
        printf("  %s motor : %d\n", i == LEFT_MOTOR ? "left" : "right",abs(counts[i]));
    }
    return 0 ;
}

/**
timedMotorDriveTest function

Initialize the motor encoder ISR and monitors the encoder counts.
Let the user select one of the motor, and either direction.
Displays the counter values for the selected motor encoder, positive for forward,
negative for backward.

@return number '0' exit code when no error found.
*/
int timedMotorDriveTest(void)
{
    char c, i, j;
    int count;
    printf("Running Timed Motor Drive Test\n");
    // Initialize motor encoder (variables, GPIO pins)
    if (init_motors() != 0)
    {
        // Handles an error from the motor initalization
        fprintf(stderr, "Unable to initialize the motors: %s\n", strerror(errno));
        fflush(stderr);
        return 1;
    }
    // Start the Interrupt Service Routines monitoring the encoders
    start_ISRs();
    // Console prompt for the motor side number
    printf("Type any motor number (left:%d - right:%d) then press ENTER... > ", LEFT_MOTOR, RIGHT_MOTOR);
    scanf(" %c", &c);
    i = c - '0';
    // Console prompt for the selected motor direction
    printf("Type direction (backward:0 - forward:1) then press ENTER... > ");
    scanf(" %c", &c);
    j = c - '0';
    // Set direction pin
    setDirection(i, j);
    // Fetch and display the current encoder count
    count = getCount(i);
    printf("  %s motor: %d\n", i == LEFT_MOTOR ? "left" : "right", count);
    // Set the motor PWM to high duty-cycle
    printf("Driving %s motor direction=%d ...\n", i == LEFT_MOTOR ? "left" : "right", getDirection(i));
    setDutyCycle(i, DEFAULT_HI_DUTY);
    // Wait for 0.5 seconds
    delay(500);
    // Display the new counter values in the console
    printf("New counts:\n");
    count = getCount(i);
    printf("  %s motor: %d\n", i == LEFT_MOTOR ? "left" : "right", count);
    // Stop the motor
    stopMotor(i);
    return 0 ;
}

/**
rampUpPwmTest function

Initialize the motor encoder ISR and monitors the encoder counts.
Let the user select one of the motor, and either direction.
Increments the motor duty-cycle by 5% every time 'ENTER' key is pressed.

@return number '0' exit code when no error found.
*/
int rampUpPwmTest(void)
{
    char i, c;
    int dutyCycle = 0;
    printf("Running Ramp Up PWM Test\n");
    // Initialize motor encoder (variables, GPIO pins)
    if (init_motors() != 0)
    {
        // Handles an error from the motor initalization
        fprintf(stderr, "Unable to initialize the motors: %s\n", strerror(errno));
        fflush(stderr);
        return 1;
    }
    // Console prompt for the motor side number
    printf("Type any motor number (left:0 - right:1) then press ENTER... > ");
    scanf(" %c", &c);
    i = c - '0';
    getchar();

    printf("Will ramp up motor %d. Press the ENTER key to increment, or 'q' then ENTER to stop...\n", i);
    // Set direction to forward
    setDirection(i, 1);
    while(dutyCycle <= 100)
    {
        printf("  motor duty-cycle at %d%%\n", dutyCycle);
        setDutyCycle(i, dutyCycle);
        dutyCycle += 5;
        // Waits for a 'ENTER'
        scanf("%c", &c);
        // Check if letter 'q' was entered, if so exit the loop
        if (c == 'q') break;
    }
    // Stop the motor
    stopMotor(i);
    return 0;
}


void proximityTest(void)
{
    printf("Running Proximity Sensor Test\n");
    // Initialize proximity sensors
    init_prox();
    printf("  Distance (in cm):\n");
    // Continuously read and display the distance of sensed object every 0.5 second
    while(1)
    {
        printf("\r          \r  %.1f", getCmDistance());
        fflush(stdout);
        delay(500);
    }
}

void IrReceiverTest(void)
{
    printf("Running IR Receiver Test\n");
    if (init_ir(1200, 8, 1, 0) != 0)
    {
        fprintf(stderr, "Unable to initialize the IR tranceiver: %s\n", strerror(errno));
        return;
    }
    setTransmitModeOff();
    while(1)
    {
        printf("\r                            \rCharacter received: %c ", getRcvLetter());
        fflush(stdout);
        delay(400);
    }
}


void IrTransmitterTest(void)
{
    char c;
    c = "$";
    printf("Running IR Transmitter Test\n");
    if (init_ir(1200, 8, 1, 0) != 0)
    {
        fprintf(stderr, "Unable to initialize the IR tranceiver: %s\n", strerror(errno));
        return;
    }
    printf("Type the letter to transmit then press ENTER... > ");
    scanf(" %c", &c);
    setXmtLetter(c);
    setTransmitModeOn();
    while(1)
    {
        delay(100);
    }
}


void IrTransceiverTest(void)
{
    char c;
    printf("Running IR Transceiver Test\n");
    if (init_ir(1200, 8, 1, 0) != 0)
    {
        fprintf(stderr, "Unable to initialize the IR tranceiver: %s\n", strerror(errno));
        return;
    }
    printf("Type the letter to transmit then press ENTER... > ");
    scanf(" %c", &c);
    setXmtLetter(c);
    while(1)
    {
        setTransmitModeOff();
        delay(100);
        printf("\r                            \rCharacter received: %c ", getRcvLetter());
        fflush(stdout);
        setTransmitModeOn();
        delay(400);
    }
}

int audioTest(void)
{
    printf("Running Audio Player test\n");
    init_player();
    int volume = 40;
    printf("Play at volume %d\n", volume);
    play_file(MP3_FILE, volume);
    volume = 80;
    printf("Play at volume %d\n", volume);
    play_file(MP3_FILE, volume);
    stopAudio();
    return 0;
}


void UDPBroadcastTest(void)
{
    char ip[20];
    printf("UDP Broadcast test\n");
    printf("Get my wireless IP: %s\n", getMyIP("wlan0"));
    printf("Enter the IP to broadcast to > ");
    fflush(stdout);
    scanf ("%s", ip);
    char* message = "Hello Everyone!";
    printf("Broadcast a UDP message: \"%s\"\n", message);
    broadcast_msg(message, ip);
}


void UDPListennerTest(void)
{
    printf("UDP Listenner test\n");
    printf("Get my wireless IP: %s\n", getMyIP("wlan0"));
    printf("Waiting for a UDP message...\n");
    const char* message = getUDPmessage();
    printf("  received: \"%s\"\n", message);
}


void control_event(int sig)
{
    printf("\n  Exiting...");
    stopAllMotors();
    stopPulse();
    stopIr();
    stopAudio();
    delay(200);
    printf(" done\n");
    exit(0);
}

void irController(void){
  //int count = 0;
  //while(1){
    IrTransmitterTest();

  //}
}
