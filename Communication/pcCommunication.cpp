#include <string>
#include <iterator>
#include "pcCommunication.h"
#include "mbed.h"

Serial pc(USBTX, USBRX,9600);
const char endCharacter = '}';

//constructors for stage
State::State(state s, int v)
{
    stateName =s;
    value = v;
}
State::State(state s)
{
    stateName = s;
    value = -1;
}

//sends state code (and value where appropriate) to the PC
void State::sendState()
{

    if(value!=-1) {
        pc.printf(
            "{\n"
            "\tstate: %s\n"
            "\tvalue: %i\n"
            "}",getStateString().c_str(), value);
    } else {
        pc.printf(
            "{\n"
            "\tstate: %s\n"
            "}",getStateString().c_str());
    }
}


string State::getStateString()
{
    if(stateName==0) {
        return "DETECTED";
    } else if(stateName== 1) {
        return "CARD_IN";
    } else if(stateName==2) {
        return "PLAYING";
    } else if(stateName==3) {
        return "OVER";
    } else if(stateName==4) {
        return "WAITING";
    }
    return "stateError";
}


void Action::determineActionType(string a)
{
    if(a=="get") {
        actionType = get;
    } else if(a=="report") {
        actionType = report;
    } else if(a=="set") {
        actionType = set;
    }
}

string Action::getActionTypeString()
{
    if(actionType==0) {
        return "get";
    } else if(actionType== 1) {
        return "set";
    } else if(actionType==2) {
        return "report";
    }
    return "actionError";
}

string Action::getDevice()
{
    std::stringstream ss;
    ss << deviceID;
    string device = deviceType+ss.str();
    return device;
}

string Action::getValue()
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

int Action::stringToInt(string text)
{
    int number;
    if(text!="") {
        std::istringstream iss(text);
        iss >> number;
        return number;
    } else {
        return NO_VALUE;
    }
}

//constructor for action
Action::Action(string pMessage[])
{
    determineActionType(pMessage[0]);
    deviceType = pMessage[1];
    deviceID = stringToInt(pMessage[2]);
    value = stringToInt(pMessage[3]); //will be -1 if no value detected
}

//determines the appropriate action based on the action content, then carries it out.
void Action::executeAction()
{

    switch(actionType) {
        case get:
            if(deviceType == "DST") { //distance sensor
                extern uint8_t distanceFromSensor;
                distanceSensor.VL6180_Init();
                pc.printf(
                    "{\n"
                    "\taction: report\n"
                    "\tdevice: DST0\n"
                    "\tvalue: %i\n"
                    "}",distanceSensor.getDistance());
            } else if(deviceType == "TCS") { //colour sensor
                rgb_sensor.enablePowerAndRGBC();
                rgb_sensor.setIntegrationTime(100);
                int red = rgb_sensor.getRedData();
                int green = rgb_sensor.getGreenData();
                int blue = rgb_sensor.getBlueData();
                int clear = rgb_sensor.getClearData();
                pc.printf(
                    "{\n"
                    "\taction: report\n"
                    "\tdevice: TCS0\n"
                    "\tvalue: %i, %i, %i, %i\n"
                    "}",red,green,blue,clear);

            } else if(deviceType == "LED") {
                pc.printf(
                    "{\n"
                    "\taction: report\n"
                    "\tdevice: LED%i\n"
                    "\tvalue: %u\n"
                    "}",deviceID,0);  //0 as a placeholder for device value, in the event the FPGA can send messages back
            } else if(deviceType == "SRV") {
                pc.printf(
                    "{\n"
                    "\taction: report\n"
                    "\tdevice: SRV%i\n"
                    "\tvalue: %u\n"
                    "}",deviceID,0);  //0 as a placeholder for device value, in the event the FPGA can send messages back
            }
            break;
        case report:
            if(deviceType == "BTN") {
                pc.printf(
                    "{\n"
                    "\taction: report\n"
                    "\tdevice: BTN%i\n"
                    "\tvalue: %u\n"
                    "}", deviceID,0);
            }
            break;
        case set:
            if(deviceType == "LED") {
                //set device method
                if(value==1) {
                    onLED(deviceID);
                } else {
                    offLED(deviceID);
                }
                pc.printf(
                    "{\n"
                    "\taction: report\n"
                    "\tdevice: LED%i\n"
                    "\tvalue: %i\n"
                    "}",deviceID,value);
            } else if(deviceType == "SRV") {
                //set device method
                if(value==1) {
                    motorUp(deviceID);
                } else {
                    motorDown(deviceID);
                }
                pc.printf(
                    "{\n"
                    "\taction: report\n"
                    "\tdevice: SRV%i\n"
                    "\tvalue: %u\n"
                    "}",deviceID,value);
            }
            break;
    }
}

void Action::printToLCD()
{
    writeLCD("a:"+ getActionTypeString(),0,0);
    writeLCD("d:"+getDevice(),0,10);
    if(value==-1) {
        writeLCD("v: n/a",1,0);
    } else {
        writeLCD("v:"+getValue(),1,0);
    }

}



string getSerialInput()
{

    char serialBuffer[128];
    memset(serialBuffer,'\0',sizeof(serialBuffer)); //sets entire serialBuffer to the empty char \0
    int i=0;
    bool endDetected=false;

    //once a character is read, it continues to read characters into the serialBuffer until the endCharacter } is read
    while(!endDetected) {
        char readCharacter = pc.getc();
        if(readCharacter!=endCharacter) {
            serialBuffer[i++]= readCharacter;
        } else {
            endDetected=true;
            break;
        }
    }
    return serialBuffer;
}

//seperates the raw serial input into seperate elements of the array
void seperateByLine(string text[])
{
    string rawSerialInput = text[0]; 
    text[0] = "";
    string parsedLine;
    int i=-1;
    bool startReading=false;
    
    //iterates through every character of the raw serial input
    for(string::iterator character = rawSerialInput.begin(); character!= rawSerialInput.end(); ++character) {
        if(*character =='\n') { //a new line character indicates to move the next element in the array.
            i++;
            startReading=false;
        } else if(*character =='\t') { //can start reading the message key and content after the indent \t character
            startReading=true;
        } else if(startReading) {
            text[i]+=*character;
        }
    }
}

//removes the key on each line, ie. "action: get" becomes "get"
void removeMessageKey(string message[], int size)
{
    //loops through every element in the array.
    for(int a=0; a<size; a++) {
        string parsedLine;
        bool spaceDetected = false;
        
        //iterates through every character in the array element
        for(string::iterator character = message[a].begin(); character!= message[a].end(); ++character) {
            
            //the below code ensures we skip the message key, as the message key is always followed by a space character.
            if(spaceDetected) {
                parsedLine+=*character;
            }
            //this is below the previous condition, as it means the space char is not included in the final string
            if(*character==' ') {
                spaceDetected = true;
            }
        }
        message[a]=parsedLine;
    }
}

//device type and id comes in single message in the form "device: [type][id]", this splits type and id into seperate strings
void seperateDeviceString(string deviceString, string &deviceName, string &deviceID)
{
    int counter=0;
    //iteraates through each character of the deviceString
    for(string::iterator character = deviceString.begin(); character!= deviceString.end(); ++character) {
        
        //the first 3 characters belong to deviceName, while the remainder belong to deviceID
        if(counter<3) {
            deviceName+=*character;
        } else {
            deviceID+=*character;
        }
        counter++;
    }
}

//parseMessage() is used to abstract the methods needed to handle parsing messages
void parseMessage(string parsedMessage[])
{
    seperateByLine(parsedMessage);
    removeMessageKey(parsedMessage,3);

    string deviceNameBuffer, deviceIDBuffer, valueBuffer=parsedMessage[2];
    seperateDeviceString(parsedMessage[1], deviceNameBuffer, deviceIDBuffer);

    //parsedMessage[0] already equals actionType
    parsedMessage[1] = deviceNameBuffer;
    parsedMessage[2] = deviceIDBuffer;
    parsedMessage[3] = valueBuffer;
}

//execute() is used to abstract the methods needed to handle messages
void execute(string message[])
{
    parseMessage(message);
    Action *newAction;
    newAction = new Action(message);
    newAction->printToLCD();
    newAction->executeAction();
    delete newAction;
}


