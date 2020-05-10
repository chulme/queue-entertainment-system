# Queue Entertainment System
This project involves 6 students from electrical and software engineering backgrounds, to develop an interactive, sensor-based system to entertain people standing in queues in a theme park. The proposed idea is an autonomous alien robot that can detect the user’s presence when they are in close proximity to it and upon pressing a button, entertain them with a game using audio, a monitor, LED indicators and two moving alien heads.

This repository concerns the embedded C++ software on the MBED microcontroller unit.

## System Overview
The system uses an MBED microcontroller, built into a university development unit. This acts as the main system driver a, and is featured in this repository, which connects to:
* PC Application - used to display misc. information eg. instructions.
* Altera Cyclone II FPGA - used as a passthrough to control output electronics such as LEDs and servo motors.
* Push-buttons (not the development unit buttons) - used as the game's primary input.
* Card sensor - for extracting the player's ID from a toothed card, using IR diodes and sensors.
* TSC34725 colour sensor.
* VL6180x Time-of-flight distance sensor.

## Modes
### Game Mode
#### Setup
The system waits until a player is detected to be within 10cm from the distance sensor. The user is then required to scan their card, read the instructions and then press any button to start the game. Failure to do any of these actions within a certain time period will caused the system to reset.

#### Gameplay
The LED array will flash in a sequence, then the user needs to remember and press the corresponding buttons in the correct order. If this is done successfully, the good alien rises and the next round begins - with each round increasing in difficulty as the sequence size increases. However, upon pressing an incorrect button, the bad alien rises and the use can see their final score on a leaderboard through the PC application.

<img src="https://i.imgur.com/B5Hc7zH.png" width="525">

### Maintenance Mode
When launched in maintenance mode, the system waits for a serial input from the PC, executes the received message, then loops back to the start waiting for the next input to be received. This allows for easy testing of the system communcation, electrical devices and the FPGA code.

From the maintenance window on the PC application, a user can:
* Request the state or measured value from the distance sensor, colour sensor, LEDs or servo motors
* Set the state of the LEDs or servo motors.

## Code

The code is split into the following modules:
* main - includes main method, initialising and linking other modules.
* modes - for determining the mode (determined by which version is launched on the PC application) and contains implementation for maintenance and game mode.
* gameLogic - the core methods used for the game mode.
* mbedDevices - for controlling devices built into the microcontroller unit.
* pcCommunication - for communicating and acting upon messages received over serial from the PC application.
* I2C_FPGA - for communicating with the FPGA using I2C.
