#include "mbedDevices.h"
#include "mbed.h"
#include "MCP23017.h"        // include 16-bit parallel I/O header file
#include "WattBob_TextLCD.h" // include 2*16 character display header file
#include "modes.h"

#define BTN1 0
#define BTN2 1


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