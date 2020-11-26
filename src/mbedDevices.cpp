#include "mbed.h"
#include "mbedDevices.h"
#include "I2C_FPGA.h"
#include <string>
#include <vector>
#include "MCP23017.h"
#include "WattBob_TextLCD.h"
#include "VL6180.h"
#include "TCS3472_I2C.h"
#include "TCS3472_I2C.h"

#define BACK_LIGHT_ON(INTERFACE) INTERFACE->write_bit(1, BL_BIT)
#define BACK_LIGHT_OFF(INTERFACE) INTERFACE->write_bit(0, BL_BIT)

#define FLASH_WAIT 0.5
#define NEXT_LED_WAIT 0.5

#define btn1 8
#define btn2 9
#define btn3 10
#define btn4 11

#define TEN_CM 100
#define RED_THRESHOLD 1000

DigitalIn cardNotch1(p21);
DigitalIn cardNotch2(p22);
DigitalIn cardNotch3(p23);
DigitalIn cardNotch4(p24);
DigitalIn cardInserted(p25);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

MCP23017 *par_port;   // pointer to 16-bit parallel I/O object
WattBob_TextLCD *lcd; // pointer to 2*16 chacater LCD object
Timer inputTimer;
VL6180  distanceSensor(p28,p27);
TCS3472_I2C rgb_sensor(p9, p10);

uint8_t distanceFromSensor = 0;


/* === LCD Methods === */
void initLCD()
{
    par_port = new MCP23017(p9, p10, 0x40); // initialise 16-bit I/O chip
    lcd = new WattBob_TextLCD(par_port);    // initialise 2*26 char display
    par_port->write_bit(1, BL_BIT);         // turn LCD backlight ON
    lcd->cls();                             // clear display
}

void clearLCD()
{
    lcd->cls(); // clear display
}

void writeLCD(string text, int xlocal, int ylocal)
{
    lcd->locate(xlocal, ylocal); // set cursor to location (0,0) - top left corner
    lcd->printf(text.c_str());   // print string
}

/* === Button Methods === */

// button release is used to prevent the user from repeatedly sending inputs when they hold the button down for more than 1 clk cycle (very small time)
//note, the code in comments is used for external buttons, whereas the running code is used for WattBob buttons.
bool getBtnReleased(/*DigitalIn btn*/ int btn)
{
    if (/*btn*/ par_port->read_bit(btn) == 1) {
        while (/*btn*/ par_port->read_bit(btn)); // loops until button is released
        return true;
    }
    return false;
}

//detects any button press from the available buttons. Will need to add to this if statement if the number of inputs increases.
int detectBtnPress()
{
    if (getBtnReleased(btn1)) {
        return 0;
    } else if (getBtnReleased(btn2)) {
        return 1;
    } else if (getBtnReleased(btn3)) {
        return 2;
    } else if (getBtnReleased(btn4)) {
        return 3;
    }
    return NO_INPUT;
}

//used to get an input (without a timeout)
int getInput()
{
    
    //loops until a button is pressed
    int input;
    do {
        input = detectBtnPress(); //if nothing inputted, input = -1, or NO_INPUT
    } while (input == NO_INPUT);
    return input;
}

//used to get an input (with a specified timeout)
int getInputWithTimeout(int timeout)
{
    int input;
    inputTimer.start();
    inputTimer.reset();
    
    //loops until a button is pressed, or the timer surpasses the specified timeout
    do {
        input = detectBtnPress(); //if nothing inputted, input = -1 (NO_INPUT)
    } while (input == NO_INPUT && inputTimer.read_ms() <= timeout);
    return input;
}

/* === LED Flash === */
void flashSequence(vector<int> sequence)
{
    //iterates through the sequence and flashes every led of the corresponding index
    for (vector<int>::iterator index = sequence.begin(); index != sequence.end(); ++index) {
        flashLED(*index);   //flashes LED (turn on, pause, turn off)
        wait(NEXT_LED_WAIT); //brief pause between each flash
    }
}

/* === Distance Sensor */
bool withinRange(int distance)
{
    if(distance<=TEN_CM) {
        return true;
    }
    return false;
}


void loopUntilWithinRange()
{
    distanceSensor.VL6180_Init();
    
    //loops until the distance sensor reads a value <= 10CM
    writeLCD("Move within 10cm",0,0);
    do {
        distanceFromSensor = distanceSensor.getDistance();
    } while(!withinRange(distanceFromSensor));
}

/* === Card Reader */

int getPlayerFromCard()
{
    int playerID = 1;
    
    //if the colour sensors doesn't detect a red object (ie. the card), return -1 indicating no card inserted, exiting the function
    if(rgb_sensor.getRedData()<=RED_THRESHOLD){  //this macro would be adjusted so it is untrue when a red card is inserted
        return -1;
    }

    //if the card notch is covered, it adds to the playerID.
    if(cardNotch1) {
        playerID+=1;
    }
    if(cardNotch2) {
        playerID+=2;
    }
    if(cardNotch3) {
        playerID+=4;
    }
    if(cardNotch4) {
        playerID+=8;
    }
    return playerID;
}
