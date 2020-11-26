#include "mbedDevices.h"
#include "mbed.h"
#include "MCP23017.h"        // include 16-bit parallel I/O header file
#include "WattBob_TextLCD.h" // include 2*16 character display header file
#include "pcCommunication.h"
#include "debugMode.h"
#include "gameMode.h"

#define BTN1 0
#define BTN2 1

//waits for the PC to send a message specifiying whether or not the MBED should run in game mode or maintenance mode
int determineMode()
{
    //instead of calling the parseMessage() method, we need to call methods manually since we do not want to seperate deviceType/ID - as this will not be sent in this message.
    string modeMessage[1];
    modeMessage[0] = getSerialInput();
    seperateByLine(modeMessage);
    removeMessageKey(modeMessage,1);
    
    //if the PC passed debug: 1, returns 1 indicating to run debugMode, otherwise return 0 indicating to run gameMode
    if(modeMessage[0]=="1") {
        return 1;
    } else {
        return 0;
    }
}

int main()
{
    initLCD();
    clearLCD();
    writeLCD("Waiting for PC..",0,0);
    if(determineMode()==0) {
        gameMode();
    } else {
        debugMode();
    }
}