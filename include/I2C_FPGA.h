#ifndef _I2C_FPGA_H
#define _I2C_FPGA_H

void onLED(int cNum);
void offLED(int cNum);
void flashLED(int cNum);
void motorUp(int cNum);
void motorDown(int cNum);
void fpgaMessage(char opCode);
void initDevices();
#endif