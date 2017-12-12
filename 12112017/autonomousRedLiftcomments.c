#pragma config(Sensor, dgtl1,  solenoid,       sensorDigitalOut)
#pragma config(Sensor, dgtl9,  leftEncoder,    sensorQuadEncoder)
#pragma config(Sensor, dgtl11, rightEncoder,   sensorQuadEncoder)
#pragma config(Motor,  port2,           DriveLeft_1,   tmotorNormal, openLoop)
#pragma config(Motor,  port3,           DriveLeft_2,   tmotorNormal, openLoop)
#pragma config(Motor,  port4,           DriveRight_1,  tmotorNormal, openLoop, reversed)
#pragma config(Motor,  port5,           DriveRight_2,  tmotorNormal, openLoop, reversed)
task resetEverything() { //this is a subroutine that will reset the motors and encoders in one go
	motor[DriveLeft_1] = 0;
	motor[DriveLeft_2] = 0;
	motor[DriveRight_1] = 0;
	motor[DriveRight_2] = 0;
	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;
}

task resetMotors() { //this will stop all of the motors
	motor[DriveLeft_1] = 0;
	motor[DriveLeft_2] = 0;
	motor[DriveRight_1] = 0;
	motor[DriveRight_2] = 0;
}

task autonomous() {
	startTask(resetEverything); //start out by resetting all of the motors/sensors
	SensorValue[solenoid] = 0; //reset the pneumatics controller
	while(SensorValue[rightEncoder] > -220) { //move the robot slightly to the left in order to get a clear path
		motor[DriveRight_1] = 27;               //to the mobile goal
		motor[DriveRight_2] = 27;
	}
	startTask(resetEverything); 
	while(SensorValue[rightEncoder] >-900 || SensorValue[leftEncoder] < 900) { //move the robot to the mobile goal
		motor[DriveLeft_1] = 63; //move at half speed
		motor[DriveLeft_2] = 63;
		motor[DriveRight_1] = 63;
		motor[DriveRight_2] = 63;
	}
	startTask(resetEverything);
	SensorValue[solenoid] = 1; //when the robot has reached the mobile goal, bring the pneumatics pistons up
	wait1Msec(1000); //wait 1 second
	while(SensorValue[leftEncoder] < -500 || SensorValue[rightEncoder] < 500) { //back out of the mobile goal area
		motor[DriveLeft_1] = -63;
		motor[DriveLeft_2] = -63;
		motor[DriveRight_1] = -63;
		motor[DriveRight_2] = -63;
	}
	startTask(resetEverything);

	while(SensorValue[leftEncoder] < 300 || SensorValue[rightEncoder] < 430) { //make a small turn to angle the robot
		motor[DriveLeft_1] = 63;                                                 //towards the scoring areas
		motor[DriveLeft_2] = 63;
		motor[DriveRight_1] = -63;
		motor[DriveRight_2] = -63;
	}
	startTask(resetMotors);
	while(SensorValue[leftEncoder] < -100 || SensorValue[rightEncoder] > 260) { //move the robot into the mobile goal area
		motor[DriveLeft_1] = 63;
		motor[DriveLeft_2] = 63;
		motor[DriveRight_1] = 63;
		motor[DriveRight_2] = 63;
	}
	startTask(resetEverything);
	while(SensorValue[rightEncoder] > -100 || SensorValue[leftEncoder] < 540) { //make another small adjustment
		motor[DriveLeft_1] = 63;
		motor[DriveLeft_2] = 63;
	}
	startTask(resetEverything);
	while(SensorValue[leftEncoder] < 350 || SensorValue[rightEncoder] > -350) { //run into the ten point scoring area
		motor[DriveLeft_1] = 63;
		motor[DriveLeft_2] = 63;
		motor[DriveRight_1] = 63;
		motor[DriveRight_2] = 63;
	}
	startTask(resetEverything);
	SensorValue[solenoid] = 0; //drop the pistons in order to score

	motor[DriveLeft_1] = -63; //back out of the scoring area at half speed for two seconds
	motor[DriveLeft_2] = -63;
	motor[DriveRight_1] = -63;
	motor[DriveRight_2] = -63;
	wait1Msec(2000);
	startTask(resetMotors); //stop all of the motors. In the competition template,
                          //manual driver control takes over after this sequence
}



task main() {
	while(true) {
		if(vexRT[Btn6U] == 1) { //start the autonomous testing program when the upper right shoulder button is pressed
			startTask(autonomous); //in the competition, this would be handled by the competition template. 
		}
	}
}
