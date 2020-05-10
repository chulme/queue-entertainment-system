#include "mbedDevices.h"
#include "gameLogic.h"
#include "pcCommunication.h"
#include "mbed.h"
#include "MCP23017.h"        // include 16-bit parallel I/O header file
#include "WattBob_TextLCD.h" // include 2*16 character display header file
#include "VL6180.h"
#include "I2C_FPGA.h"
#include "modes.h"
#include "TCS3472_I2C.h"

#define GAME_TIME_OUT 2000 //ms for time out (2000ms=2s)
#define START_TIME_OUT 15000 //ms for start time out (10s)
#define CARD_TIME_OUT 15000 //ms for start time out (10s)

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

//waits for a player to be within 10cm from the unit
void waitForPlayer()
{
    loopUntilWithinRange(); //waits until the sensor detects an object within range
    writeLCD("in range",0,0);

    //Send state playerDetected to PC
    State *playerDetected = new State(State::playerDetected);
    playerDetected->sendState();
    delete playerDetected;
}


//waits for a certain period for the user to insert a card. If no card is inserted
//this method times out and returns false, resulting in the MBED to wait for a new player to trip the distance sensor
bool cardInserted()
{
    writeLCD("card detect",0,0);

    Timer inputTimer;

    int playerID;
    rgb_sensor.enablePowerAndRGBC();
    rgb_sensor.setIntegrationTime(100);
    inputTimer.start();
    inputTimer.reset();
    
    //loops until a card is inserted or until timeout
    do {
        playerID = getPlayerFromCard(); //if nothing inputted, input = -1 (NO_INPUT)
    } while (playerID == NO_INPUT && inputTimer.read_ms() <= CARD_TIME_OUT);
    
    //if nothing inputed, return false and exit the function
    if(playerID==-1) { 
        return false;
    } else {
        //Send state cardDeteced to PC
        State *cardDetected = new State(State::cardDetected, playerID);
        cardDetected->sendState();
        delete cardDetected;
        clearLCD();

        /* === Print playerID to LCD for debugging === */
        char id[1];
        sprintf(id, "%d", playerID);
        string line = "id: ";
        line += id;
        writeLCD(line, 0, 0);
        wait(2);
        clearLCD();
        return true;
    }
}

//waits for a certain period for the user to press any button. If no button is pressed
//this method times out and returns false, resulting in the MBED to wait for a new player to trip the distance sensor
bool startInputPressed()
{
    writeLCD("press btn",0,0);
    int input = getInputWithTimeout(START_TIME_OUT);
    if(input == -1) {
        return false;
    }
    //Send state playing to PC
    State *playing = new State(State::playing);
    playing->sendState();
    delete playing;
    return true;
}


void gameOver(int score)
{
    clearLCD();
    writeLCD("Game Over!", 0, 0);
    motorUp(BAD_ALIEN);

    //Send gameOver playing to PC
    State *gameOver = new State(State::gameOver,score);
    gameOver->sendState();
    delete gameOver;
}

void gameMode()
{
    while(1) {
        /* === init === */
        State *waiting = new State(State::waiting);
        waiting->sendState();
        delete waiting;

        clearLCD();
        initDevices();
        srand(time(NULL));
        int sequenceLength = 1;
        int score=0;
        vector<int> buttonSequence;
        bool gameRunning = true;
        
        /* === pre game control flow == */
        waitForPlayer();
        clearLCD();
        if(!cardInserted()) {
            clearLCD();
            writeLCD("Timed out.",0,0);
            wait(5);
            continue; //goes back the start of the loop
        }


        clearLCD();
        if(!startInputPressed()) {
            clearLCD();
            writeLCD("Timed out.",0,0);
            wait(2);
            continue; //goes back to the start of the loop
        } else {
            /* === game loop === */
            clearLCD();
            State *playing = new State(State::playing);
            playing->sendState();
            delete waiting;
            
            while (gameRunning) {
                
                generateNewSequence(buttonSequence, sequenceLength);
                printSequence(buttonSequence);
                flashSequence(buttonSequence);

                //for every led flashes, the user should press the corresponding button
                for (vector<int>::iterator index = buttonSequence.begin(); index != buttonSequence.end(); ++index) {
                    int input = getInputWithTimeout(GAME_TIME_OUT);
                    
                    //print input to LCD (debugging)
                    char s[1];
                    sprintf(s, "%d", input + 1);
                    string op = "Last In: ";
                    op += s;
                    writeLCD(op, 1, 0);
                    
                    
                    if (!inputCorrect(input, *index)) {
                        wait(1);
                        gameRunning = false;
                        break;
                    }
                }
                if (gameRunning) {
                    score += buttonSequence.size() * 10;
                    motorUp(GOOD_ALIEN);
                    wait(2);
                    motorDown(GOOD_ALIEN);
                }
                sequenceLength++;   //increments the sequence length each round
            }


            gameOver(score);
            wait(1);
            clearLCD();

            /* === Print score to LCD === */
            char scr[1]; //INCREASE THIS
            sprintf(scr, "%d", score);
            string op = "Fin Scr: ";
            op += scr;
            writeLCD(op, 0, 0);
            wait(5);
        }
    }
}