#pragma config(Sensor, in4,    rightShoulderPot, sensorPotentiometer)
#pragma config(Sensor, in5,    rightElbowPot,  sensorPotentiometer)
#pragma config(Sensor, dgtl1,  liftSolenoid,   sensorDigitalOut)
#pragma config(Sensor, dgtl2,  clawSolenoid,   sensorDigitalOut)
#pragma config(Sensor, I2C_1,  rightIME,       sensorNone)
#pragma config(Sensor, I2C_2,  leftIME,        sensorNone)
#pragma config(Sensor, I2C_3,  armIME,         sensorNone)
#pragma config(Motor,  port1,           leftFrontDriveMotor, tmotorVex393_HBridge, openLoop, driveLeft)
#pragma config(Motor,  port3,           rightFrontDriveMotor, tmotorVex393_MC29, openLoop, driveRight)
#pragma config(Motor,  port4,           rightElbowMotor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           rightRearDriveMotor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           rightShoulderMotor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           leftShoulderMotor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          leftRearDriveMotor, tmotorNone, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int deadSpot = 15;
int desiredShoulderPotValue = 0;
int desiredElbowPotValue =0;
unsigned long programTimeAtLastJoystickCheck = 0;
int shoulderPotFullyForwardValue = 990;
int shoulderPotFullyBackwardValue = 3600;
int elbowPotFullyForwardValue = 9;
int elbowPotFullyBackwardValue = 50;
int millisecondsForFullShoulderTravel = 2000;

void tankDrive() {
	// Right drive
	int rightDrivePower = 0;
	if (abs(vexRT[Ch2]) > deadSpot) {
		rightDrivePower = vexRT[Ch2] * -1;
	}
	motor[rightFrontDriveMotor] = rightDrivePower;
	motor[rightRearDriveMotor] = rightDrivePower;

	// Left drive
	int leftDrivePower = 0;
	if (abs(vexRT[Ch3]) > deadSpot) {
		leftDrivePower = vexRT[Ch3];
	}
	motor[leftFrontDriveMotor] = leftDrivePower;
	motor[leftRearDriveMotor] = leftDrivePower;
}

void arcadeDriveFromLeftJoystick() {
  int forwardness = 0;
  if (abs(vexRT[Ch3]) > deadSpot) {
  	forwardness = vexRT[Ch3];
  };
  int rightness = 0;
  if (abs(vexRT[Ch4]) > deadSpot) {
  	rightness = vexRT[Ch4];
  };
  // Right drive
	int rightDrivePower = (forwardness - rightness) / 2 * -1;
	motor[rightFrontDriveMotor] = rightDrivePower;
	motor[rightRearDriveMotor] = rightDrivePower;

	// Left drive
	int leftDrivePower = (forwardness + rightness) / 2;
	motor[leftFrontDriveMotor] = leftDrivePower;
	motor[leftRearDriveMotor] = leftDrivePower;
}

// Function to calculate where the user would like the pot to be, from where they have set the joystick
int calculateNewDesiredPotValue(
	int currentDesiredPotValue,
	float fractionOfFullMovementTime,
	int joystickChannel,
	int minPotValue,
	int maxPotValue
){

  int newDesiredPotValue = currentDesiredPotValue;

	if (abs(vexRT[joystickChannel]) > deadSpot) {
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

void armControlFromRightJoystick(){

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
	int shoulderMotorError = desiredShoulderPotValue - SensorValue[rightShoulderPot];
	int shoulderMotorPower = shoulderMotorError / 10;

	// Try to limit juddering - maybe try hysteresis?
	if (shoulderMotorPower > 100) shoulderMotorPower = 100;

	motor[rightShoulderMotor] = shoulderMotorPower;
	motor[leftShoulderMotor] = shoulderMotorPower;

	// Same again for elbow motor
	int elbowMotorError = desiredElbowPotValue - SensorValue[rightElbowPot];
	int elbowMotorPower = elbowMotorError * 5;

	// Try to limit juddering - maybe try hysteresis?
	if (elbowMotorPower > 100) elbowMotorPower = 100;

	motor[rightElbowMotor] = elbowMotorPower;
}

task main()
{
	// Initialise desired arm pot values to current values
  desiredShoulderPotValue = SensorValue[rightShoulderPot];
  desiredElbowPotValue = SensorValue[rightElbowPot];

	while(true) {
		// Check if we're in close-up mode or not
		if (vexRT[Btn5U]) {
			arcadeDriveFromLeftJoystick();
			armControlFromRightJoystick();
			} else {
			tankDrive();
		}
		moveOrHoldPotsAtDesiredPositions();
	}
}
