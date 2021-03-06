# Queue Entertainment System
This project is an interactive, sensor-based system to entertain people standing in queues in a theme park. The proposed idea is an autonomous alien robot that can detect the user’s presence when they are in close proximity to it and upon pressing a button, entertain them with a game using audio, a monitor, LED indicators and two moving alien heads.

## System Overview
The system uses an MBED microcontroller, built into a university development unit. This acts as the main system driver a, and is featured in this repository, which connects to:
* PC Application - used to display misc. information eg. instructions.
* Altera Cyclone II FPGA - used as a passthrough to control output electronics such as LEDs and servo motors.
* Push-buttons (not the development unit buttons) - used as the game's primary input.
* Card sensor - for extracting the player's ID from a toothed card, using IR diodes and sensors.
* TSC34725 colour sensor.
* VL6180x Time-of-flight distance sensor.

## Gameplay

The LED array will flash in a sequence, then the user needs to remember and press the corresponding buttons in the correct order. If this is done successfully, the good alien rises and the next round begins - with each round increasing in difficulty as the sequence size increases. However, upon pressing an incorrect button, the bad alien rises and the use can see their final score on a leaderboard through the PC application.

<img src="https://i.imgur.com/B5Hc7zH.png" width="525">

## Code
The code is split into the following modules:
* main - includes main method, where it determines and executes the correct mode.
* gameMode - contains implementation for game mode.
* debugMode - contains implementation for maintenance.
* mbedDevices - for controlling devices built into the microcontroller unit.
* pcCommunication - for communicating and acting upon messages received over serial from the PC application.
* I2C_FPGA - for communicating with the FPGA using I2C (developed by a project team-member).
