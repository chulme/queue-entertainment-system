#include <vector>
#include <iterator>
#include <iostream>
#include <time.h>
#include <sstream>
#include <string>

#include "gameLogic.h"
#include "mbedDevices.h"
#include "pcCommunication.h"
#include "mbed.h"
#include "I2C_FPGA.h"
#include "modes.h"

#define NUMBEROFINPUTS 4
#define GAME_TIME_OUT 2000 //ms for time out (2000ms=2s)
#define START_TIME_OUT 15000 //ms for start time out (10s)
#define CARD_TIME_OUT 15000 //ms for start time out (10s)

using namespace std;

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
        
        if(!pregameSetupComplete()){
            continue;
        }

        if(!startInputPressed()) {
            clearLCD();
            writeLCD("Timed out.",0,0);
            wait(2);
            continue;
        }
            /* === game loop === */
            State *playing = new State(State::playing);
            playing->sendState();
            delete playing;
            
            int sequenceLength = 1;
            int score=0;
            bool gameRunning = true;

            while (gameRunning) {
                
                vector<int> buttonSequence = generateNewSequence(sequenceLength);
                printSequence(buttonSequence);
                flashSequence(buttonSequence);

                //for every led flashes, the user should press the corresponding button
                for (vector<int>::iterator index = buttonSequence.begin(); index != buttonSequence.end(); ++index) {
                    int input = getInputWithTimeout(GAME_TIME_OUT);
                                        
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
        
    }
}

//Adds a random number from 0 to 5 to the vector
vector<int> generateNewSequence(const int length)
{
    vector<int> btnSequence;
    for (int i = 0; i < length; i++) {
        int randIndex = rand() % NUMBEROFINPUTS;
        btnSequence.push_back(randIndex);
    }
    return btnSequence;
}

//Prints every element in vector
void printSequence(vector<int> btnSequence)
{
    writeLCD("Sqn:", 0, 0);
    int local = 5;
    
    //iterates through the btnSequence and prints the each btnIndex
    for (vector<int>::iterator btnIndex = btnSequence.begin(); btnIndex != btnSequence.end(); ++btnIndex) {
        char s[1];
        sprintf(s, "%d", *btnIndex + 1);
        writeLCD(s, 0, local);
        local++;
    }
}

bool inputCorrect(int input, int index)
{
    if (input == index) {
        return true;
    } else {
        return false;
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
    clearLCD();
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
    clearLCD();
}

bool pregameSetupComplete(){
        waitForPlayer();
        clearLCD();
        if(!cardInserted()) {
            clearLCD();
            writeLCD("Timed out.",0,0);
            wait(5);
            clearLCD();
            return false;
        }else{
            return true;
        }

}


