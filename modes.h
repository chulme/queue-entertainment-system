#define IDENTIFICATIONMODEL_ID 0x0000
#define TEN_CM 100
#define BTN1 0
#define BTN2 1

#define GOOD_ALIEN 0
#define BAD_ALIEN 1
int determineMode();

void debugMode();

void gameMode();

void waitForPlayer();

void waitForCard();

bool startInputPressed();

void gameOver();