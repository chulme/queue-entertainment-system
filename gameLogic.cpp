#include <vector>
#include <iterator>
#include <iostream>
#include <time.h>
#include <sstream>
#include <string>

#include "gameLogic.h"
#include "mbedDevices.h"

#define NUMBEROFINPUTS 4

using namespace std;

//Adds a random number from 0 to 5 to the vector
void generateNewSequence(vector<int> &btnSequence, int length)
{
    btnSequence.clear(); //resets sequence
    //loops from 0 to length, generates a random number from 0 to NUMBEROFINPUTS and adds it to the btnSequence
    for (int i = 0; i < length; i++) {
        int randIndex = rand() % NUMBEROFINPUTS;
        btnSequence.push_back(randIndex);
    }
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