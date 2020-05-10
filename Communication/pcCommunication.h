#ifndef pcCommunication_H
#define pcCommunication_H
#include <sstream>
#include <string>
#include "mbed.h"
#include "MCP23017.h"        // include 16-bit parallel I/O header file
#include "WattBob_TextLCD.h" // include 2*16 character display header file
#include "mbedDevices.h"
#include "I2C_FPGA.h"
#include "VL6180.h"
#include "TCS3472_I2C.h"


using namespace std;

#define NO_VALUE -1

extern Serial pc;
extern VL6180 distanceSensor;
extern TCS3472_I2C rgb_sensor;

string getSerialLine();
string getSerialInput();

void execute(string message[]);

void seperateByLine(string message[]);

void removeMessageKey(string message[], int size);

void seperateDeviceString(string deviceString, string &deviceName, string &deviceID);

void parseMessage(string parsedMessage[]);

enum act {get = 0, set = 1, report = 2};


class State
{

public:
    enum state {playerDetected = 0, cardDetected = 1, playing = 2, gameOver = 3, waiting = 4};

    //constructors for stage
    State(state s, int v);
    State(state s);
    
    //sends state code (and value where appropriate) to the PC
    void sendState();

private:
    state stateName;
    int value;

    string getStateString();

};
class Action
{
private:
    act actionType;
    string deviceType;
    int deviceID;
    int value;

    void determineActionType(string a);

    string getActionTypeString();

    string getDevice();

    string getValue();
    
    int stringToInt(string text);
    
public:

    //constructor for action
    Action(string pMessage[]);

    //determines the appropriate action based on the action content, then carries it out.
    void executeAction();

    void printToLCD();
};
#endif
