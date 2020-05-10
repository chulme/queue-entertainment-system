#include <vector>

using namespace std;

//Adds a random number from 0 to 5 to the vector
void generateNewSequence(vector<int> &btnSequence, int length);

//Prints every element in vector
void printSequence(vector<int> btnSequence);

int getBtnInput();

bool inputCorrect(int input, int index);