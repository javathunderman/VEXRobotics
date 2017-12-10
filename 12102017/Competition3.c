#pragma config(Sensor, dgtl1,  solenoid,       sensorDigitalOut)
#pragma config(Sensor, dgtl9,  leftEncoder,    sensorQuadEncoder)
#pragma config(Sensor, dgtl11, rightEncoder,   sensorQuadEncoder)
#pragma config(Motor,  port2,           DriveLeft_1,   tmotorNormal, openLoop)
#pragma config(Motor,  port3,           DriveLeft_2,   tmotorNormal, openLoop)
#pragma config(Motor,  port4,           DriveRight_1,  tmotorNormal, openLoop, reversed)
#pragma config(Motor,  port5,           DriveRight_2,  tmotorNormal, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)
#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!
#pragma platform(VEX2)


/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////


void pre_auton()
{
	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;
	SensorValue[solenoid] = 0;
}
task resetEverything() {
	motor[DriveLeft_1] = 0;
	motor[DriveLeft_2] = 0;
	motor[DriveRight_1] = 0;
	motor[DriveRight_2] = 0;
	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;

}

task resetMotors() {
	motor[DriveLeft_1] = 0;
	motor[DriveLeft_2] = 0;
	motor[DriveRight_1] = 0;
	motor[DriveRight_2] = 0;
}
task autonomous() {
	startTask(resetEverything);
	SensorValue[solenoid] = 0;
	while(SensorValue[rightEncoder] > -220) {
		motor[DriveRight_1] = 27;
		motor[DriveRight_2] = 27;
	}
	startTask(resetMotors);
	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;
	while(SensorValue[rightEncoder] >-900 || SensorValue[leftEncoder] < 900) {
		motor[DriveLeft_1] = 63;
		motor[DriveLeft_2] = 63;
		motor[DriveRight_1] = 63;
		motor[DriveRight_2] = 63;
	}
	startTask(resetMotors);
	SensorValue[solenoid] = 1;
	wait1Msec(1000);


	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;
	while(SensorValue[leftEncoder] < -500 || SensorValue[rightEncoder] < 500) {
		motor[DriveLeft_1] = -63;
		motor[DriveLeft_2] = -63;
		motor[DriveRight_1] = -63;
		motor[DriveRight_2] = -63;
	}
	startTask(resetEverything);

	while(SensorValue[leftEncoder] < 300 || SensorValue[rightEncoder] < 430) {//total shot in the dark
		motor[DriveLeft_1] = 63;
		motor[DriveLeft_2] = 63;
		motor[DriveRight_1] = -63;
		motor[DriveRight_2] = -63;
	}
	startTask(resetMotors);
	while(SensorValue[leftEncoder] < -100 || SensorValue[rightEncoder] > 260) {
		motor[DriveLeft_1] = 63;
		motor[DriveLeft_2] = 63;
		motor[DriveRight_1] = 63;
		motor[DriveRight_2] = 63;
	}
	startTask(resetEverything);
	while(SensorValue[rightEncoder] > -90) {
		motor[DriveLeft_1] = 63;
		motor[DriveLeft_2] = 63;
	}
	startTask(resetEverything);
	while(SensorValue[leftEncoder] < 600 || SensorValue[rightEncoder] < -600) {
		motor[DriveLeft_1] = 63;
		motor[DriveLeft_2] = 63;
		motor[DriveRight_1] = 63;
		motor[DriveRight_2] = 63;
	}
	startTask(resetEverything);
	SensorValue[solenoid] = 0;
	while(SensorValue[leftEncoder] < -300 || SensorValue[rightEncoder] > 300) {
		motor[DriveLeft_1] = -63;
		motor[DriveLeft_2] = -63;
		motor[DriveRight_1] = -63;
		motor[DriveRight_2] = -63;
	}
}



/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////


#define     JOY_DRIVE_V     vexJSLeftV
#define     JOY_DRIVE_H     vexJSLeftH
#define     JOY_THRESHOLD   15

#define MOTOR_NUM               kNumbOfTotalMotors
#define MOTOR_MAX_VALUE         127
#define MOTOR_MIN_VALUE         (-127)
#define MOTOR_DEFAULT_SLEW_RATE 10      // Default will cause 375mS from full fwd to rev
#define MOTOR_FAST_SLEW_RATE    256     // essentially off
#define MOTOR_TASK_DELAY        15      // task 1/frequency in mS (about 66Hz)
#define MOTOR_DEADBAND          10

// Array to hold requested speed for the motors
int motorReq[ MOTOR_NUM ];

// Array to hold "slew rate" for the motors, the maximum change every time the task
// runs checking current mootor speed.
int motorSlew[ MOTOR_NUM ];


task MotorSlewRateTask()
{
	unsigned int motorIndex;
	int motorTmp;

	// Initialize stuff
	for(motorIndex=0;motorIndex<MOTOR_NUM;motorIndex++)
	{
		motorReq[motorIndex] = 0;
		motorSlew[motorIndex] = MOTOR_DEFAULT_SLEW_RATE;
	}

	// run task until stopped
	while( true )
	{
		// run loop for every motor
		for( motorIndex=0; motorIndex<MOTOR_NUM; motorIndex++)
		{
			// So we don't keep accessing the internal storage
			motorTmp = motor[ motorIndex ];

			// Do we need to change the motor value ?
			if( motorTmp != motorReq[motorIndex] )
			{
				// increasing motor value
				if( motorReq[motorIndex] > motorTmp )
				{
					motorTmp += motorSlew[motorIndex];
					// limit
					if( motorTmp > motorReq[motorIndex] )
						motorTmp = motorReq[motorIndex];
				}

				// decreasing motor value
				if( motorReq[motorIndex] < motorTmp )
				{
					motorTmp -= motorSlew[motorIndex];
					// limit
					if( motorTmp < motorReq[motorIndex] )
						motorTmp = motorReq[motorIndex];
				}

				// finally set motor
				motor[motorIndex] = motorTmp;
			}
		}

		// Wait approx the speed of motor update over the spi bus
		wait1Msec( MOTOR_TASK_DELAY );
	}
}


void
DriveLeftMotor( int value )
{
	motorReq[ DriveLeft_1 ] = value;
	motorReq[ DriveLeft_2 ] = value;
}


void
DriveRightMotor( int value )
{
	motorReq[ DriveRight_1 ] = value;
	motorReq[ DriveRight_2 ] = value;
}


task ArcadeDrive()
{
	int    ctl_v;
	int    ctl_h;
	int    drive_l;
	int    drive_r;

	// Basic arcade control
	while( true )
	{
		// Get joystick H and V control
		ctl_v = vexRT[ JOY_DRIVE_V ];
		ctl_h = vexRT[ JOY_DRIVE_H ];

		// Ignore joystick near center
		if( (abs(ctl_v) <= JOY_THRESHOLD) && (abs(ctl_h) <= JOY_THRESHOLD) )
		{
			drive_l = 0;
			drive_r = 0;
		}
		else
		{
			// Create drive for left and right motors
			drive_l = (ctl_v + ctl_h) / 2;
			drive_r = (ctl_v - ctl_h) / 2;
		}

		// send out to motors
		DriveLeftMotor( drive_l );
		DriveRightMotor( drive_r );

		// Cortex is shit
		wait1Msec( 25 );
	}
}
task usercontrol()
{
	// Start motor slew rate control
	startTask(MotorSlewRateTask);

	// Start driver control tasks
	startTask(ArcadeDrive);

	// Start Newmatics
	

	// Everything done in other tasks
	while( true )
	{
		hack(NSA);
		seal(governemnt_secrets);
		SensorValue[solenoid] = vexRT[Btn6U];
		wait1Msec( 100 );
	}
}
