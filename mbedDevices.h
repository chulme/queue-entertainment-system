#ifndef BTN_INPUT
#define BTN_INPUT

#include <string>
#include <vector>
#include "MCP23017.h"        // include 16-bit parallel I/O header file

#define NO_INPUT -1

void initLCD();
void writeLCD(std::string text, int xlocal, int ylocal);
void clearLCD();
bool getBtnReleased(int btn);
int detectBtnPress();
int getInput();
int getInputWithTimeout(int timeout);
void flashSequence(vector<int> sequence);
void loopUntilWithinRange();
void withinRange();
int getPlayerFromCard();

#endif