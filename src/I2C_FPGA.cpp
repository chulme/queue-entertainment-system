#include "mbed.h"
#include "I2C_FPGA.h"
#include "mbedDevices.h"

I2C i2c(p28, p27); //Define I2C pins

const int adrsFPGA = 0x02; //Address for FPGA

//Initialise LED Opcodes
const char onLED1 = 0x21;
const char onLED2 = 0x83;
const char onLED3 = 0x85;
const char onLED4 = 0xC7;
const char onLED5 = 0x29;
const char onLED6 = 0x3C;

const char offLED1 = 0x02;
const char offLED2 = 0x64;
const char offLED3 = 0x36;
const char offLED4 = 0x08;
const char offLED5 = 0x6A;
const char offLED6 = 0x0C;

//Initialise Motor Opcodes
const char motorUp1 = 0x91;
const char motorUp2 = 0xD3;

const char motorDown1 = 0x12;
const char motorDown2 = 0xB5;

void onLED(int cNum)
{
    switch (cNum) //Switch statement switching on the LED chosen
    {
    case 0:
        fpgaMessage(onLED1); //Send message to FPGA to turn on LED 1
        break;
    case 1:
        fpgaMessage(onLED2);
        break;
    case 2:
        fpgaMessage(onLED3);
        break;
    case 3:
        fpgaMessage(onLED4);
        break;
    case 4:
        fpgaMessage(onLED5);
        break;
    case 5:
        fpgaMessage(onLED6);
        break;
    case 6:
        for (int i = 0; i < 6; i++) //Loop for turning on all LED's
        {
            onLED(i);
        }
        break;
    }
}

void offLED(int cNum)
{
    switch (cNum)
    {
    case 0:
        fpgaMessage(offLED1); //Send message to FPGA to turn off LED 1
        break;
    case 1:
        fpgaMessage(offLED2);
        break;
    case 2:
        fpgaMessage(offLED3);
        break;
    case 3:
        fpgaMessage(offLED4);
        break;
    case 4:
        fpgaMessage(offLED5);
        break;
    case 5:
        fpgaMessage(offLED6);
        break;
    case 6:
        for (int i = 0; i < 6; i++) //Loop for turning off all LED's
        {
            offLED(i);
        }
        break;
    }
}

void flashLED(int cNum)
{
    onLED(cNum);
    wait(0.2);
    offLED(cNum);
}

void motorUp(int cNum)
{
    switch (cNum) //Switch statement switching on the Motor chosen
    {
    case 0:
        fpgaMessage(motorUp1);
        break;
    case 1:
        fpgaMessage(motorUp2);
        break;
    case 2:
        fpgaMessage(motorUp1); //Move motor 1 & 2 up
        fpgaMessage(motorUp2);
        break;
    }
}

void motorDown(int cNum)
{
    switch (cNum)
    {
    case 0:
        fpgaMessage(motorDown1);
        break;
    case 1:
        fpgaMessage(motorDown2);
        break;
    case 2:
        fpgaMessage(motorDown1); //Move motor 1 & 2 down
        fpgaMessage(motorDown2);
        break;
    }
}

void fpgaMessage(char opCode)
{

    char message[2];
    message[0] = opCode;

    i2c.write(adrsFPGA, message, 1); //Send message to the FPGA with the given Opcode
    i2c.write(adrsFPGA, message, 1); //Repeat the message to make sure it was recieved properly
}

void initDevices()
{
    offLED(0);
    offLED(1);
    offLED(2);
    offLED(3);
    offLED(4);
    offLED(5);
    motorDown(0);
    motorDown(1);
}