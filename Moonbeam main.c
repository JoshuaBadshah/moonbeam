#pragma config(Sensor, in1,    testbutton,     sensorAnalog)
#pragma config(Sensor, in4,    Pot1,           sensorPotentiometer)
#pragma config(Sensor, in5,    Pot2,           sensorPotentiometer)
#pragma config(Sensor, dgtl1,  goalLift,       sensorDigitalOut)
#pragma config(Sensor, dgtl2,  claw,           sensorDigitalOut)
#pragma config(Motor,  port1,           leftfrontdrive, tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port3,           rightfrontdrive, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port4,           armfront,      tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5,           rightbackdrive, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port8,           rightarmbottom, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port9,           leftarmbottom, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port10,          leftbackdrive, tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int threshold = 15;
bool goalPosition = false;
bool held = true;
bool taskOneActive = false;

void Sounds()
{
playSoundFile("EstablishingLi.wav");
wait1Msec(1000);
playSoundFile("LinkageComp.wav");
wait1Msec(500);
playSoundFile("ProgramComp.wav");
wait1Msec(1000);
playSoundFile("ProgramReady");
}

void SelfTest()
{
	playSoundFile("DiagnosticStart.wav");
	wait1Msec(3000);
	motor[leftbackdrive] = 100;
	motor[leftfrontdrive] = 100;
	motor[rightbackdrive] = 100;
	motor[rightfrontdrive] = 100;
	wait1Msec(1000);
	motor[leftbackdrive] = 0;
	motor[leftfrontdrive] = 0;
	motor[rightbackdrive] = 0;
	motor[rightfrontdrive] = 0;
	wait1Msec(1000);
	motor[leftbackdrive] = -100;
	motor[leftfrontdrive] = -100;
	motor[rightbackdrive] = -100;
	motor[rightfrontdrive] = -100;
	wait1Msec(1000);
	motor[leftbackdrive] = 0;
	motor[leftfrontdrive] = 0;
	motor[rightbackdrive] = 0;
	motor[rightfrontdrive] = 0;
	wait1Msec(1500);
	motor[leftbackdrive] = 100;
	motor[leftfrontdrive] = 100;
	motor[rightbackdrive] = -100;
	motor[rightfrontdrive] = -100;
	wait1Msec(1000);
	motor[leftbackdrive] = 0;
	motor[leftfrontdrive] = 0;
	motor[rightbackdrive] = 0;
	motor[rightfrontdrive] = 0;
	wait1Msec(1500);
	motor[leftbackdrive] = -100;
	motor[leftfrontdrive] = -100;
	motor[rightbackdrive] = 100;
	motor[rightfrontdrive] = 100;
	wait1Msec(1000);
	motor[leftbackdrive] = 0;
	motor[leftfrontdrive] = 0;
	motor[rightbackdrive] = 0;
	motor[rightfrontdrive] = 0;
	wait1Msec(1000);
	SensorValue(goalLift) = 1;
	wait1Msec(1000);
	SensorValue(goalLift) = 0;
	wait1Msec(1000);


}

task One()
{
	taskOneActive = true;
	while(SensorValue[Pot1] < 3000 && held == false)
	{
		motor[leftarmbottom] = 20;
		motor[rightarmbottom] = 20;
	}
	motor[leftarmbottom] = 0;
	motor[rightarmbottom] = 0;
	taskOneActive = false;
}



task main()//blah
{
	Sounds();
	while(1) {
		if(abs(vexRT[Ch3]) > threshold)         // If the left joystick is greater than or less than the threshold:
		{
			motor[leftbackdrive]  = (vexRT[Ch3]);
			motor[leftfrontdrive]  = (vexRT[Ch3]);
		}
		else                                    // If the left joystick is within the threshold:
		{
			motor[leftbackdrive]  = 0;                // Stop the left motor (cancel noise)
			motor[leftfrontdrive]  = 0;
		}

		if(abs(vexRT[Ch2]) > threshold)         // If the right joystick is greater than or less than the threshold:
		{
			motor[rightbackdrive] = (vexRT[Ch2]);
			motor[rightfrontdrive]  = (vexRT[Ch2]);
		}
		else                                    // If the right joystick is within the threshold:
		{
			motor[rightbackdrive] = 0;                // Stop the right motor (cancel noise)
			motor[rightfrontdrive]  = 0;
		}
		if (vexRT[Btn5U] == 1) {
			held = true;
			motor[leftarmbottom] = 90;
			motor[rightarmbottom] = 90;
		}
		else if (vexRT[Btn5D] == 1) {
			held = true;
			motor[leftarmbottom] = -40;
			motor[rightarmbottom] = -40;
		}
		else if(vexRT[Btn5U] == 0 && vexRT[Btn5D] == 0) {
			held = false;
			if (taskOneActive == false){
				startTask(One);
			}
		}
		if (vexRT[Btn6U] == 1) {
			motor[armfront] = 90;
		}
		else if (vexRT[Btn6D] == 1) {
			motor[armfront] = -60;
		}
		else {
			motor[armfront] = 10;
		}
		if (vexRT[Btn7U] == 1) {
			SensorValue[claw] = 1;
		}
		else if (vexRT[Btn7D] == 1) {
			SensorValue[claw] = 0;
		}

		if (vexRT[Btn8U] && goalPosition == false) {
			goalPosition = true;
			SensorValue(goalLift) = 1;
			waitUntil(vexRT[Btn8U] == 0);
		}
		else if (vexRT[Btn8U] && goalPosition == true) {
			goalPosition = false;
			SensorValue(goalLift) = 0;
			waitUntil(vexRT[Btn8U] == 0);
		}
		if (SensorValue[testbutton] < 5) {
			// Do self-test regime
		playSoundFile("ButtonPress.wav");
			SelfTest();
				playSoundFile("DiagComplete.wav");


		}
	}
}
