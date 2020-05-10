# Queue Entertainment System
This project involves 6 students from electrical and software engineering backgrounds, to develop an interactive, sensor-based system to entertain people standing in queues in a theme park. The proposed idea is an autonomous alien robot that can detect the user’s presence when they are in close proximity to it and upon pressing a button, entertain them with a game using audio, a monitor, LED indicators and two moving alien heads.

# System Overview


# Modes

## Game Mode
### Setup
The system waits until a player is detected to be within 10cm from the distance sensor. The user is then required to scan their card, read the instructions and then press any button to start the game. Failure to do any of these actions within a certain time period will caused the system to reset.

### Gameplay
The LED array will flash in a sequence, then the user needs to remember and press the corresponding buttons in order. 
The system will generate an integer pattern from 1-6, with the pattern length depending on the current level. These


## Maintenance Mode
When launched in maintenance mode, the system waits for a serial input from the PC, executes the received message, then loops back to the start waiting for the next input to be received. This allows for easy testing of the system communcation, electrical devices and the FPGA code.

From the maintenance window on the PC application, a user can:
* Request the state or measured value from the distance sensor, colour sensor, LEDs or servo motors
* Set the state of the LEDs or servo motors.
