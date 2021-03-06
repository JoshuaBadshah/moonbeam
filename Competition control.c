#pragma config(Sensor, in4,    PotRightShoulder, sensorPotentiometer)
#pragma config(Sensor, in6,    PotLeftShoulder, sensorPotentiometer)
#pragma config(Sensor, in7,    PotRightElbow,  sensorPotentiometer)
#pragma config(Sensor, in8,    PotLeftElbow,   sensorPotentiometer)
#pragma config(Sensor, dgtl1,  goalLift,       sensorDigitalOut)
#pragma config(Sensor, dgtl2,  claw,           sensorDigitalOut)
#pragma config(Motor,  port1,           RightBottomArmMotor, tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           RightBackMotor, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port4,           RightFrontMotor, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5,           LeftFrontArmMotor, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port6,           RightFrontArmMotor, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port7,           LeftBackMotor, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port8,           LeftFrontMotor, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port10,          LeftBottomArmMotor, tmotorNone, openLoop)
// This code is for the VEX cortex platform
#pragma platform(VEX2)

// Select Download method as "competition"
#pragma competitionControl(Competition)

//Main competition background code...do not modify!
#include "Vex_Competition_Includes.c"

// int statement used for numbers.
int threshold = 15; // setting variable to stop toggle from making noise if not set to zero.
// Bool statement used for true and false.
bool GoalPosition = false; // this variable will decide whether the goal is up or down.
bool ClawPosition = false; // this variable will decide whether claw is open or closed
bool held = true; //if the arm needs to be held in position or not
unsigned long programTimeAtLastJoystickCheck = 0; //this will be used to calculate whether the code has been re-checked
int desiredShoulderPotValue = 0; // will be set to whatever position the arm is at the start
int desiredElbowPotValue = 0; // will be set to whatever position the elbow is at the start
int elbowPotFullyForwardValue = 10;
int elbowPotFullyBackwardValue = 4000;
int shoulderPotFullyForwardValue = 625;
int shoulderPotFullyBackwardValue = 3200;
int millisecondsForFullShoulderTravel = 2000;

void NormalTankDrive(){ // setting the task for normal tank drive, it will be using two toggles

	//Right side drive. Motors will be moving in opposite direction to left side.
	int RightDriveSpeed = 0; // setting start speed to zero.
	if (abs(vexRT[Ch2]) > threshold) { // the toggle is outside of the toggle range
		RightDriveSpeed = vexRT[Ch2] * -1; // the negative means the motor will be moving opposite to left side
	}
	// however far the toggle is pushed, both motors will recieve same data and will therefore move at the same speed
	motor[RightFrontMotor] = RightDriveSpeed;
	motor[RightBackMotor] = RightDriveSpeed;

	//Left side drive. Motors will be moving in opposite direction to right side.
	int LeftDriveSpeed = 0; // setting start speed to zero.
	if (abs(vexRT[Ch3]) > threshold) { // the toggle is outside of the toggle range
		LeftDriveSpeed = vexRT[Ch3]; // no negative sign needed. Data taken from left toggle.
	}
	// however far the toggle is pushed, both motors will recieve same data and will therefore move at the same speed
	motor[LeftFrontMotor] = LeftDriveSpeed;
	motor[LeftBackMotor] = LeftDriveSpeed;

}


void NormalArmControlOverRide() { // will be used to specify where the arm roughly needs to be. Arms moving full speed
	// arm going up
	if (vexRT[Btn5U] == 1) {
			held = true; // signalling that the arm should not be held
			motor[LeftBottomArmMotor] = -90;
			motor[RightBottomArmMotor] = -90;
		}
	// arm going down
	else if (vexRT[Btn5D] == 1) {
		held = true;
		motor[LeftBottomArmMotor] = 40;
		motor[RightBottomArmMotor] = 40;
	}
	else if(vexRT[Btn5U] == 0 && vexRT[Btn5D] == 0) {
		held = false; // signalling arm should be held

	}

  // this for the the seondary arm
	if (vexRT[Btn6U] == 1) { //arm moves up
			motor[RightFrontArmMotor] = 90;
			motor[LeftFrontArmMotor] = 90;
		}
	else if (vexRT[Btn6D] == 1) { // arm moves down
	  	motor[RightFrontArmMotor] = -60;
	  	motor[LeftFrontArmMotor] = -60;
		}
	else{
		motor[RightFrontArmMotor] = 0;
		motor[LeftFrontArmMotor] = 0;
		}
}

void arcadeDriveFromLeftJoystick() { // this will be used to precisely pick up the cones.

	int forwardness = 0;
  if (abs(vexRT[Ch3]) > threshold) {
  	forwardness = vexRT[Ch3]; // set fowardness value to how far the toggle is pushed
  };
  int rightness = 0;
  if (abs(vexRT[Ch4]) > threshold) {
  	rightness = vexRT[Ch4]; // set fowardness value to how far the toggle is pushed
  };
  // Right drive
	int rightDrivePower = (forwardness - rightness) / 2 * -1;
	motor[RightFrontMotor] = rightDrivePower;
	motor[RightBackMotor] = rightDrivePower;

	// Left drive
	int leftDrivePower = (forwardness + rightness) / 2;
	motor[LeftFrontMotor] = leftDrivePower;
	motor[LeftBackMotor] = leftDrivePower;
}

// this function changes the arm angle to the pot value that is now desired.
int calculateNewDesiredPotValue(
	int currentDesiredPotValue,
	float fractionOfFullMovementTime,
	int joystickChannel,
	int minPotValue,
	int maxPotValue
){

	int newDesiredPotValue = currentDesiredPotValue;//this is the pot value before the arm has been auto corrected
	if (abs(vexRT[joystickChannel]) > threshold) {
		float fractionJoystick = (float)vexRT[joystickChannel] / (float)127;
		float fractionJoystickInverted = fractionJoystick * -1;
		int potRange = maxPotValue - minPotValue;
		float desiredPotChangeInValue = fractionJoystickInverted * (float)potRange;
		desiredPotChangeInValue = desiredPotChangeInValue * fractionOfFullMovementTime ;
	  newDesiredPotValue = currentDesiredPotValue + desiredPotChangeInValue;
  }

  // Make sure we don't overshoot
  if (newDesiredPotValue < minPotValue) { newDesiredPotValue = minPotValue; }
  if (newDesiredPotValue > maxPotValue) { newDesiredPotValue = maxPotValue; }

  return newDesiredPotValue;
}

void armControlFromRightJoystick(){// stating the values for the rightjoystick. Note; no motor values occur here

	// Figure out how long since we were last here
	unsigned long millisecondsSinceLastJoystickCheck = nSysTime - programTimeAtLastJoystickCheck;
	if (millisecondsSinceLastJoystickCheck > 100) {
		programTimeAtLastJoystickCheck = nSysTime;

		// Figure out how much to move in this time slice
		float fractionOfFullMovementTime = (float)millisecondsSinceLastJoystickCheck / (float)millisecondsForFullShoulderTravel;
		// Figure out what the user would like the shoulder pot value to be
		desiredShoulderPotValue = calculateNewDesiredPotValue(
			desiredShoulderPotValue,
			fractionOfFullMovementTime,
			Ch2,
			shoulderPotFullyForwardValue,
			shoulderPotFullyBackwardValue
		);

		desiredElbowPotValue = calculateNewDesiredPotValue(
			desiredElbowPotValue,
			fractionOfFullMovementTime,
			Ch1,
			elbowPotFullyForwardValue,
			elbowPotFullyBackwardValue
		);
	}
}

void moveOrHoldPotsAtDesiredPositions() {
	// Adjust the shoulder motor power to try to get the shoulder pot to where it should be
	int shoulderMotorError =  SensorValue[PotLeftShoulder] - desiredShoulderPotValue;
	int shoulderMotorPower = shoulderMotorError / 9;

	// Try to limit juddering - maybe try hysteresis?
	if (shoulderMotorPower > 100) shoulderMotorPower = 100;

	motor[LeftBottomArmMotor] = shoulderMotorPower;
	motor[RightBottomArmMotor] = shoulderMotorPower;

	// Same again for elbow motor
	int elbowMotorError = desiredElbowPotValue - SensorValue[PotLeftElbow];
	int elbowMotorPower = elbowMotorError / 8;


	// Try to limit juddering - maybe try hysteresis?
	if (elbowMotorPower > 100) elbowMotorPower = 100;

	motor[LeftFrontArmMotor] = elbowMotorPower;
}


void ClawAndMobileGoalLiftPNEMATICS(){ // pnematics use Sensor values. And function binary(1,0).

	if (vexRT[Btn8L] && GoalPosition == false) { // if the button is clicked and variable GoalPosition is set to false.
			GoalPosition = true; //change variable to true to the elif command can be initialised.
			SensorValue(goalLift) = 1; //the solenoid will be set to 1. So the goal will be lifted.
			waitUntil(vexRT[Btn8L] == 0);
		}
	else if (vexRT[Btn8L] && GoalPosition == true) { //if clicked again since variable was set to true this loop begins
			GoalPosition = false; // set back to false so that the next loop can be used when clicked again
			SensorValue(goalLift) = 0; // the solenoid will be set to 0. So the goal drops.
			waitUntil(vexRT[Btn8L] == 0);
	  }

	 if (vexRT[Btn8R] && ClawPosition == false) { // if the button is clicked and variable GoalPosition is set to false.
			ClawPosition = true; //change variable to true to the elif command can be initialised.
			SensorValue(claw) = 0; //the solenoid will be set to 1.
			waitUntil(vexRT[Btn8R] == 0);
		}

	 else if (vexRT[Btn8R] && ClawPosition == true) { //if clicked again since variable was set to true this loop begins
			ClawPosition = false; // set back to false so that the next loop can be used when clicked again
			SensorValue(claw) = 1; // the solenoid will be set to 0.
			waitUntil(vexRT[Btn8R] == 0);
	  }

}



void pre_auton()
{
  // Set bStopTasksBetweenModes to false if you want to keep user created tasks
  // running between Autonomous and Driver controlled modes. You will need to
  // manage all user created tasks if set to false.
  bStopTasksBetweenModes = true;

	// Set bDisplayCompetitionStatusOnLcd to false if you don't want the LCD
	// used by the competition include file, for example, you might want
	// to display your team name on the LCD in this function.
	// bDisplayCompetitionStatusOnLcd = false;

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

task HoldArms(){ //This is used to hold the arms in place if no buttons are pressed
	while(SensorValue[PotRightShoulder] > 1500) // as long as no buttons are pressed and the angle is right
	{
		// this will provide the upward force to hold the arm
		motor[LeftBottomArmMotor] = -20;
		motor[RightBottomArmMotor] = -20;
	}
	motor[LeftBottomArmMotor] = 0;
	motor[RightBottomArmMotor] = 0;
}

task autonomous() //Autonomous with 7682E Drive back 10sec
{
 	SensorValue[goalLift] = 0;
	wait1Msec(20);
	SensorValue[goalLift] = 1;
	wait1Msec(20);
	SensorValue[goalLift] = 0;
	wait1Msec(20);
	motor[LeftFrontArmMotor] = -40;
	wait1Msec(700);
	motor[LeftFrontArmMotor] = 0;
	wait1Msec(20);
	motor[LeftBottomArmMotor] = -80;
	motor[RightBottomArmMotor] = -80;
	wait1Msec(1700);
	startTask(HoldArms);
	wait1Msec(20);
	motor[LeftFrontArmMotor] = -70;
	wait1Msec(500);
	motor[LeftFrontArmMotor] = 0;
	wait1Msec(800);

}



task usercontrol()
{


  desiredShoulderPotValue = SensorValue[PotLeftShoulder];
  desiredElbowPotValue = SensorValue[PotLeftElbow];

	while(true) {
		// Check if we're in close-up mode or not
		if (vexRT[Btn6U]) {
			arcadeDriveFromLeftJoystick();
			armControlFromRightJoystick();
			} else {
			NormalTankDrive();
		}
		moveOrHoldPotsAtDesiredPositions();
		ClawAndMobileGoalLiftPNEMATICS();
  }
}
