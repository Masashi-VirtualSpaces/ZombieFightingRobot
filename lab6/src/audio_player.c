#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h> 

#include "pinout.h"
#include "audio_player.h"

void init_player(void)
{
    pinMode(OUT_AUDIO_EN, OUTPUT);
    // Turn OFF audio by default
    digitalWrite(OUT_AUDIO_EN, LOW);
}

int play_file(char* filename, int volume)
{
    // Check the volume level is >0 to 100
    if (volume > 0 && volume <= 100)
    {
        // Approximate convert to millibel
        int millibel = (volume * 60) - 5000;
        // Set the system command to play audio
        char command[200];
        sprintf(command, "omxplayer -o local --vol %d %s  > /dev/null 2>&1", millibel, filename);
        // Turn ON audio chip
        digitalWrite(OUT_AUDIO_EN, HIGH);
        // Execute the player command
        int ret = system(command);
        if(ret)
        {
            // Handle error in command
            fprintf(stderr,"Error executing system command %s return code: %d\n", command, ret);
            fflush(stderr);
            stopAudio();
            return ret;
        }
    }
    else
    {
        // Handle invalid volume
        fprintf(stderr,"Error in volume function argument, must be 1 to 100\n");
        fflush(stderr);
        stopAudio();
        return 1;
    }
    return 0;
}


void stopAudio(void)
{
    digitalWrite(OUT_AUDIO_EN, LOW);
}
