#ifndef GAME_MODE
#define GAME_MODE

#include <vector>

using namespace std;

vector<int> generateNewSequence(const int length);
void printSequence(vector<int> btnSequence);
int getBtnInput();
bool inputCorrect(int input, int index);

void gameMode();
void waitForPlayer();
void waitForCard();
bool startInputPressed();
void gameOver();
bool pregameSetupComplete();

#endif