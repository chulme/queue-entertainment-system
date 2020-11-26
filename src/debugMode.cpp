#include "mbedDevices.h"
#include "pcCommunication.h"
#include "mbed.h"
#include "debugMode.h"
#include "gameMode.h"

#define GAME_TIME_OUT 2000 //ms for time out (2000ms=2s)
#define START_TIME_OUT 15000 //ms for start time out (10s)
#define CARD_TIME_OUT 15000 //ms for start time out (10s)

void debugMode()
{
    clearLCD();
    initDevices();
    writeLCD("Waiting for in..",0,0);
    
    //loops forever (until the MBED is reset). Waits for a message from the PC then executes it.
    while(1) {
        string message[4];
        message[0] = getSerialInput();
        clearLCD();
        execute(message);
    }

}

