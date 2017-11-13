#pragma config(UART_Usage, UART2, uartNotUsed, baudRate4800, IOPins, None, None)
#pragma config(Sensor, in1,    clawPot,        sensorPotentiometer)
#pragma config(Sensor, dgtl1,  llift,          sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  rlift,          sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  ldrive,         sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  rdrive,         sensorQuadEncoder)
#pragma config(Sensor, dgtl10, bump,           sensorTouch)
#pragma config(Motor,  port2,           lDrive2,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           lLift,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           lLift1,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           rClaw,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           lClaw,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           rLift1,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           rLift,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           rDrive2,       tmotorVex393HighSpeed_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*        Description: Competition template for VEX EDR                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// This code is for the VEX cortex platform
#pragma platform(VEX2)

// Select Download method as "competition"
#pragma competitionControl(Competition)

#pragma autonomousDuration(15)

//Main competition background code...do not modify!
#include "Vex_Competition_Includes.c"


//Encoder heights (to be filled)
int fullDown = 0;
int lowFence;
int highFence;
int forwardDump = 64;
int backwardDump = 157;
int backwardClaw;
int encMax;
int encMin = 0;

float kLift = 5;
float kDrive = 2;
float kDriveForward = 2.5;

int liftError = 4;
int driveError = 5;
int driveForwardError = 3;

int lastLiftPower = 0;
int lastLiftPos = 0;
int lastClawPower = 0;
int lastClawPos = 0;

bool bumpFollow = false;
int downPower = -15;

bool settingLift = false;
bool settingClaw = false;

bool isDumping = false;
bool dumped = false;

int autoClawPower = 0;
int autoLiftPower = 0;

int C1, C2, C3, C4, B5D, B5U, B6D, B6U, B7D, B7U, B7L, B7R, B8D, B8U, B8L, B8R;

//Claw task vars
int lastClaw = 0;

int limit(int in, int lim)
{
	return in>127?127:in<-127?-127:in;
}

void setDrive(int l, int r)
{
	int q = l;
	l = r;
	r = q;
	float lf=l;
	float rf =r;
	//lf*=.75;
	//rf*=.75;
	//motor[rDrive1] = -(rf>127?127:rf<-127?-127:rf);
	motor[rDrive2] = -(rf>127?127:rf<-127?-127:rf);
	//motor[lDrive1] = (lf>127?127:lf<-127?-127:lf);
	motor[lDrive2] = (lf>127?127:lf<-127?-127:lf);

}

int getLLiftEnc()
{
	return SensorValue[llift];
}

int getRLiftEnc()
{
	return SensorValue[rlift];
}

int getAvgLiftEnc()
{
	return (getLLiftEnc()+getRLiftEnc())/2;
}

void setLift(int power)
{
	//power = -power;
	float powerf=(float)power;
	//powerf*=.75;
	//motor[lLowLift] = (powerf>127?127:powerf<-127?-127:powerf);
	motor[lLift] = -(powerf>127?127:powerf<-127?-127:powerf);
	motor[lLift1] = -(powerf>127?127:powerf<-127?-127:powerf);
	//motor[rLowLift] = -(power>127?127:power<-127?-127:power);
	motor[rLift] = (powerf>127?127:powerf<-127?-127:powerf);
	motor[rLift1] = (powerf>127?127:powerf<-127?-127:powerf);

}

void setClaw(int power)
{
	power = -power;
	float powerf=power;
	//powerf*=.5;
	motor[rClaw] = (powerf>127?127:powerf<-127?-127:powerf);
	motor[lClaw] = -(powerf>127?127:powerf<-127?-127:powerf);
}

void setClawTo(int pos)
{
	settingClaw = true;
	clearTimer(T2);
	if (SensorValue[clawPot]<pos)
	{
		while(SensorValue[clawPot]<pos)
		{
			setClaw(127);
			if(time1[T2]>2500)
			{
				return;
			}
		}
	}
	else if (SensorValue[clawPot]>pos)
	{
		setClaw(-127);
		if(time1[T2]>2500)
		{
			return;
		}
	}
	setClaw(0);
	settingClaw = false;
	lastClawPower = 0;
	lastClawPos = SensorValue[clawPot];
}

task setClawPos()
{
	wait1Msec(250);
	lastClawPos = SensorValue[clawPot];
}

void clawMaster(int power)
{
	if(settingClaw)
	{
		return;
	}
	if(power!=0)
	{
		setClaw(power);
		lastClawPower = power;
	}
	else
	{
		if(lastClawPower!=0)
		{
			lastClawPower = 0;
			lastClawPos = SensorValue[clawPot];
		}
		else
		{
			if(SensorValue[clawPot]-lastClawPos>20)//claw too closed
			{
				setClaw(-((float)(SensorValue[clawPot]-lastClawPos))*.1);
			}
			else if (SensorValue[clawPot]-lastClawPos<-20)//lift too low
			{
				setClaw(-((float)(SensorValue[clawPot]-lastClawPos))*.1);
			}
			else
			{
				setClaw(0);
			}
		}
	}
}

void clawMS(int power, int time)
{
	setClaw(power);
	wait1Msec(time);
	setClaw(0);
}



void driveMS(int l, int r, int time)
{
	setDrive(l, r);
	wait1Msec(time);
	setDrive(0,0);
}

void driveForwardMS(int power, int time)
{
	writeDebugStream("Driving forward for %d", time);
	writeDebugStreamLine(" at %d power.", power);
	driveMS(power, power, time);
	writeDebugStreamLine("Finished Drive");
}


void turnMS(int power, int time)
//+power is clockwise
{
	setDrive(power, -power);
	wait1Msec(time);
	setDrive(0,0);
}



void setLiftTo(int enc)
{
	settingLift = true;
	bumpFollow = false;
	writeDebugStreamLine("Setting lift to enc value: %d", enc);
	if(getAvgLiftEnc()<enc)
	{
		while(abs(enc-getAvgLiftEnc())>liftError)
		{
			setLift(-(getAvgLiftEnc()-enc)*kLift);
			writeDebugStream("Current enc: %d", getAvgLiftEnc());
			writeDebugStream(" Target: %d", enc);
			writeDebugStreamLine(" Motor power: %d", (getAvgLiftEnc()-enc)*kLift);
		}
	}
	else if(abs(enc-getAvgLiftEnc())>liftError)
	{
		while(getAvgLiftEnc()>enc)
		{
			setLift(-(getAvgLiftEnc()-enc)*kLift);
			writeDebugStream("Current enc: %d", getAvgLiftEnc());
			writeDebugStream(" Target: %d", enc);
			writeDebugStreamLine(" Motor power: %d", (getAvgLiftEnc()-enc)*kLift);
		}
	}
	else
	{

	}
	setLift(0);
	writeDebugStreamLine("Finished set lift.");
	settingLift = false;
	lastLiftPower = 0;
	lastLiftPos = getAvgLiftEnc();
}

void liftMaster(int power)
{
	if(settingLift)
	{
		return;
	}
	if(power!=0)
	{
		if (SensorValue[bump]==1&&power<0)
		{
			setLift(downPower);
		}
		else
		{
			setLift(power);
		}
		lastLiftPower = power;
		if (power > 0)
		{
			bumpFollow = false;
			downPower = -15;
		}
	}
	else
	{
		if(lastLiftPower!=0)
		{
			lastLiftPower = 0;
			lastLiftPos = getAvgLiftEnc();
		}
		else if(bumpFollow)
		{
			if(SensorValue[bump]==1)
			{
				lastLiftPos = getAvgLiftEnc();
			}
			else
			{
				downPower-=.2;
			}
			setLift(downPower);
		}
		else if(SensorValue[bump]==1&&lastLiftPower<=0&&getAvgLiftEnc()<100)
		{
			bumpFollow = true;
			setLift(downPower);
		}
		else
		{
			if(getAvgLiftEnc()-lastLiftPos>25)//lift too high
			{
				setLift(-((float)(getAvgLiftEnc()-lastLiftPos))*.08);
			}
			else if (getAvgLiftEnc()-lastLiftPos<-25)//lift too low
			{
				setLift(-((float)(getAvgLiftEnc()-lastLiftPos))*.25);
			}
			else
			{
				setLift(0);
			}
		}
	}
}

int getAvgDriveEnc()
{
	return (SensorValue[ldrive]+SensorValue[rdrive])/2;
}

int getAvgTurnDrive()
{
	return (SensorValue[ldrive]-SensorValue[rdrive])/2;
}

int ticksToDeg(int ticks)
{
	return (int)ticks*4*3.1415/16;
}

void turnDeg(float deg)
{
	//Positive degrees = clockwise
	//deg = -deg;
	clearTimer(T1);
	SensorValue[ldrive] = 0;
	SensorValue[rdrive] = 0;
	writeDebugStreamLine("Turning %d degrees", deg);
	if(deg>0)
	{
		while(deg-ticksToDeg(getAvgTurnDrive())>driveError)
		{
			int error = deg-ticksToDeg(getAvgTurnDrive());
			setDrive(-kDrive*error, kDrive*error);
			writeDebugStream("Current deg: %d", ticksToDeg(getAvgTurnDrive()));
			writeDebugStream(" Target: %d", deg);
			writeDebugStreamLine(" Motor power: %d", kDrive*error);
			if (time1[T1]>1500)
			{
				return;
			}
			if(abs(kDrive*error)<15)
			{
				break;
			}
		}
	}
	else if(deg<0)
	{
		while(ticksToDeg(getAvgTurnDrive())-deg>driveError)
		{
			int error = ticksToDeg(getAvgTurnDrive())-deg;
			setDrive(kDrive*error, -kDrive*error);
			writeDebugStream("Current deg: %d", ticksToDeg(getAvgTurnDrive()));
			writeDebugStream(" Target: %d", deg);
			writeDebugStreamLine(" Motor power: %d", -kDrive*error);
			if (time1[T1]>1500)
			{
				return;
			}
			if(abs(kDrive*error)<15)
			{
				break;
			}
		}
	}
	else
	{

	}
	setDrive(0,0);
	writeDebugStreamLine("Finished turning %d degrees.", deg);
}

void drivePrecise(int in)
{
	SensorValue[ldrive] = 0;
	SensorValue[rdrive] = 0;
	writeDebugStreamLine("Driving %d inches", in);
	if(in>0)
	{
		while(in-(getAvgDriveEnc()*4*3.14/360)>driveForwardError)
		{
			float error = in-(getAvgDriveEnc()*4*3.14/360);
			setDrive(kDriveForward*error, kDriveForward*error);
			writeDebugStream("Current inches: %d", getAvgDriveEnc()*4*3.14/360);
			writeDebugStream(" Target: %d", in);
			writeDebugStreamLine(" Motor power: %d", kDriveForward*error);
		}
	}
	else if(in<0)
	{
		while((getAvgDriveEnc()*4*3.14/360)-in>driveForwardError)
		{
			int error = (getAvgDriveEnc()*4*3.14/360)-in;
			setDrive(-kDriveForward*error, -kDriveForward*error);
			writeDebugStream("Current inches: %d", (getAvgDriveEnc()*4*3.14/360));
			writeDebugStream(" Target: %d", in);
			writeDebugStreamLine(" Motor power: %d", kDriveForward*error);
		}
	}
	else
	{

	}
	writeDebugStreamLine("Finished driving %d inches.", in);
	setDrive(0,0);
}

void driveRightSide(int deg)
{
	SensorValue[rdrive] = 0;
	writeDebugStreamLine("Driving right %d degrees", deg);
	int target = deg*16/4*2.5;
	if(deg>0)
	{
		while((target-SensorValue[rdrive])>30)
		{
			int error = target-SensorValue[rdrive];
			setDrive(3*error, 0);
			writeDebugStream("Current inches: %d", SensorValue[rdrive]);
			writeDebugStream(" Target: %d", target);
			writeDebugStreamLine(" Motor power: %d",1.5*error);
		}
	}
	/*else if(deg<0)
	{
	while((getAvgDriveEnc()*4*3.14/360)-in>driveForwardError)
	{
	int error = (getAvgDriveEnc()*4*3.14/360)-in;
	setDrive(-kDriveForward*error, -kDriveForward*error);
	writeDebugStream("Current inches: %d", (getAvgDriveEnc()*4*3.14/360));
	writeDebugStream(" Target: %d", in);
	writeDebugStreamLine(" Motor power: %d", kDriveForward*error);
	}
	}*/
	/*else
	{

	}*/
	writeDebugStreamLine("Finished driving %d degrees right", deg);
	setDrive(0,0);
}

void driveLeftSide(int deg)
{
	SensorValue[ldrive] = 0;
	writeDebugStreamLine("Driving left %d degrees", deg);
	int target = deg*16/4*2.5;
	if(deg>0)
	{
		while((target-SensorValue[ldrive])>30)
		{
			int error = target-SensorValue[ldrive];
			setDrive(0, 3*error);
			writeDebugStream("Current inches: %d", SensorValue[ldrive]);
			writeDebugStream(" Target: %d", target);
			writeDebugStreamLine(" Motor power: %d",3*error);
		}
	}
	else if(deg<0)
	{
		while((target-SensorValue[ldrive])<-30)
		{
			int error = target-SensorValue[ldrive];
			setDrive(0, 3*error);
			writeDebugStream("Current inches: %d", SensorValue[ldrive]);
			writeDebugStream(" Target: %d", target);
			writeDebugStreamLine(" Motor power: %d",3*error);
		}
	}
	else
	{

	}
	writeDebugStreamLine("Finished driving %d degrees left", deg);
	setDrive(0,0);
}

void noPIDDrive(int in, int power)
{
	int encTarget = in/4/3.14*360;
	clearTimer(T1);
	SensorValue[ldrive] = 0;
	SensorValue[rdrive] = 0;
	int encValue = (SensorValue[rdrive]+SensorValue[ldrive])/2;
	if (in>0)
	{
		while(encTarget-encValue>0)
		{
			setDrive(power, power);
			if (time1[T1]>3000)
			{
				return;
			}
			encValue = (SensorValue[rdrive]+SensorValue[ldrive])/2;
		}
	}
	else if (in<0)
	{
		while(encTarget-encValue<0)
		{
			setDrive(-power, -power);
			if (time1[T1]>3000)
			{
				return;
			}
			encValue = (SensorValue[rdrive]+SensorValue[ldrive])/2;
		}
	}
	else
	{

	}
	setDrive(0,0);
}

void straightPIDDrive(int in)
{
	int encTarget = in/4/3.14*360;
	clearTimer(T1);
	SensorValue[ldrive] = 0;
	SensorValue[rdrive] = 0;
	int encValue = (SensorValue[rdrive]+SensorValue[ldrive])/2;
	if (in>0)
	{
		while(encTarget-SensorValue[rdrive]>0&&encTarget-SensorValue[ldrive]>0)
		{
			setDrive(127*(encTarget-SensorValue[rdrive])*.2, 127*(encTarget-SensorValue[ldrive])*.2);
			if (time1[T1]>3000)
			{
				return;
			}
			encValue = (SensorValue[rdrive]+SensorValue[ldrive])/2;
		}
	}
	else if (in<0)
	{
		while(encTarget-SensorValue[rdrive]<0&&encTarget-SensorValue[ldrive]<0)
		{
			setDrive(127*(encTarget-SensorValue[rdrive])*.2, 127*(encTarget-SensorValue[ldrive])*.2);
			if (time1[T1]>3000)
			{
				return;
			}
			encValue = (SensorValue[rdrive]+SensorValue[ldrive])/2;
		}
	}
	else
	{

	}
	setDrive(0,0);
}

// paste this function above the pre-autonomous task in your competition template code
void replayVexRT(int C1, int C2, int C3, int C4, int B5D, int B5U, int B6D, int B6U, int B7D, int B7U, int B7L, int B7R, int B8D, int B8U, int B8L, int B8R)
{
	int ry = C2;
	int ly = C3;
	setDrive(ry, ly);
	int up = B5U>0?1:0;
	int down = B5D>0?1:0;
	int clawOut = B6U;
	int clawIn = B6D;
	int	lift = up+down==2?0:up==1?1:down==1?-1:0;
	setLift(lift*127);
	int claw = clawOut+clawIn==2?0:clawOut==1?1:clawIn==1?-1:20.0/127.0;
	setClaw(claw*127);
	wait1Msec(100);
}

task recordVexRT()
{
	while(true)
	{
		C1 = vexRT[Ch1];
		C2 = vexRT[Ch2];
		C3 = vexRT[Ch3];
		C4 = vexRT[Ch4];
		B5D = vexRT[Btn5D];
		B5U = vexRT[Btn5U];
		B6D = vexRT[Btn6D];
		B6U = vexRT[Btn6U];
		B7D = vexRT[Btn7D];
		B7U = vexRT[Btn7U];
		B7L = vexRT[Btn7L];
		B7R = vexRT[Btn7R];
		B8D = vexRT[Btn8D];
		B8U = vexRT[Btn8U];
		B8L = vexRT[Btn8L];
		B8R = vexRT[Btn8R];

		writeDebugStream("replayVexRT(%d", C1);
		writeDebugStream(",%d", C2);
		writeDebugStream(",%d", C3);
		writeDebugStream(",%d", C4);
		writeDebugStream(",%d", B5D);
		writeDebugStream(",%d", B5U);
		writeDebugStream(",%d", B6D);
		writeDebugStream(",%d", B6U);
		writeDebugStream(",%d",	B7D);
		writeDebugStream(",%d", B7U);
		writeDebugStream(",%d", B7L);
		writeDebugStream(",%d", B7R);
		writeDebugStream(",%d", B8D);
		writeDebugStream(",%d", B8U);
		writeDebugStream(",%d", B8L);
		writeDebugStream(",%d", B8R);
		writeDebugStreamLine(");");

		wait1Msec(100);
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

task autoLift()
{
	while(true)
	{
		liftMaster(autoLiftPower);
		wait1Msec(20);
	}
}

task autoClaw()
{
	while(true)
	{
		clawMaster(autoClawPower);
		wait1Msec(20);
	}
}

task clawDump()
{
	while(1)
	{
		if(getAvgLiftEnc()>750&&lastClawPos<1100)
		{
			lastClawPos = 1500;
		}
		wait1Msec(20);
	}
}

void clearAllSensors()
{
	SensorValue[rdrive] = 0;
	SensorValue[ldrive] = 0;
	SensorValue[rlift] = 0;
	SensorValue[llift] = 0;
}

void liftFullDown()
{
	lastLiftPos = -1000;
	while(SensorValue[bump]==0)
	{

	}
}

void deploy()
{
	setClawTo(1000);
	wait1Msec(50);
	setLiftTo(500);
	liftFullDown();
}


void skillsDeploy()
{
	/*setClawTo(320);
	setLiftTo(400);*/
}

void back()
{
	setClawTo(650);
	lastClawPos = 750;
	noPIDDrive(-26, 127);
	setDrive(-127, -127);
	setLiftTo(1000);
	setDrive(0,0);
	wait1Msec(1000);
}

void forth()
{
	setClawTo(600);
	setLiftTo(400);
	noPIDDrive(36, 80);
	setDrive(0, 0);
	setClawTo(450);
}

void rightCubeAuto()
{
	deploy();
	wait1Msec(250);
	noPIDDrive(26, 127);
	setDrive(0,0);
	setClawTo(400);
	setLiftTo(300);
	turnDeg(-135);
	setDrive(-127, -127);
	lastLiftPos = 1350;
	while(SensorValue[rlift]+SensorValue[llift]<2*1200)
	{

	}
	lastLiftPower = 0;
	wait1Msec(50);
	autoLiftPower = 0;
	setDrive(0, 0);
	setClawTo(500);
}

void leftCubeAuto()

{
	deploy();
	noPIDDrive(26, 127);
	setDrive(0,0);
	setClawTo(400);
	liftFullDown();
	turnDeg(135);
	setDrive(-127, -127);
	lastLiftPos = 1350;
	while(SensorValue[rlift]+SensorValue[llift]<2*1200)
	{

	}
	lastLiftPower = 0;
	wait1Msec(50);
	autoLiftPower = 0;
	setDrive(0, 0);
	setClawTo(500);
}

void threeStarAuto()
{
	liftFullDown();
	setClawTo(1200);
	noPIDDrive(26, 80);
	setDrive(0,0);
	setClawTo(450);
	setLiftTo(300);
	setDrive(-127, -127);
	lastLiftPos = 1350;
	while(SensorValue[rlift]+SensorValue[llift]<2*1200)
	{

	}
	lastLiftPower = 0;
	wait1Msec(50);
	autoLiftPower = 0;
	setDrive(0, 0);
	setClawTo(500);
}

void fenceStarAuto()
{
	setClawTo(1650);
	setLiftTo(200);
	liftFullDown();
	setLiftTo(475);
	setDrive(127, 127);
	wait1Msec(3500);
	setDrive(0,0);
	setClawTo(400);
	wait1Msec(750);
	setClawTo(1750);

}

void dumpStart()
{
	setClawTo(320);
	noPIDDrive(-12, 90);
	setDrive(80, 80);
	wait1Msec(500);
	setDrive(0,0);
	lastClawPos = 700;
	noPIDDrive(-6, 30);
	setClawTo(570);
	noPIDDrive(6,60);
	setClawTo(650);
	lastClawPos = 700;
	setLiftTo(350);
	noPIDDrive(-24, 127);
	setDrive(-127, -127);
	setLiftTo(1000);
	setDrive(0,0);
	setClawTo(650);
	setLiftTo(750);
	setClawTo(320);
	setLiftTo(1000);
}

void dumpAuto()
{
	dumpStart();
	liftFullDown();
}

void rightDumpCube()
{
	driveLeftSide(68);
	noPIDDrive(26, 80);
	lastClawPos = 700;
	wait1Msec(250);
	setLiftTo(450);
	turnDeg(-60);
	noPIDDrive(-12, 127);
	setDrive(-127, -127);
	setLiftTo(1000);
	setDrive(0,0);
	setClawTo(500);
}

void leftDumpCube()
{
	driveRightSide(68);
	noPIDDrive(26, 80);
	lastClawPos = 700;
	wait1Msec(250);
	setLiftTo(450);
	turnDeg(60);
	noPIDDrive(-12, 127);
	setDrive(-127, -127);
	setLiftTo(1000);
	setDrive(0,0);
	setClawTo(500);
}

void autoSkills()
{
	/*dumpStart();
	setLiftTo(400);
	noPIDDrive(36, 127);*/
	skillsDeploy();
	wait1Msec(1000);
	back();
	forth();
	wait1Msec(1000);
	back();
	setClawTo(450);
	wait1Msec(500);
	setLiftTo(100);
	driveLeftSide(105);
	liftFullDown();
	setClawTo(425);
	noPIDDrive(30, 80);
	setDrive(0,0);
	lastClawPos = 700;
	wait1Msec(500);
	setLiftTo(500);
	turnDeg(-60);
	wait1Msec(500);
	noPIDDrive(-15, 127);
	setDrive(-127, -127);
	setLiftTo(1000);
	setDrive(0,0);
	setClawTo(500);
	wait1Msec(500);
	setLiftTo(100);
	driveLeftSide(68);
	liftFullDown();
	noPIDDrive(30, 80);
	setClawTo(700);
	setLiftTo(450);
	turnDeg(-60);
	noPIDDrive(-18, 127);
	setDrive(-127, -127);
	setLiftTo(1000);
	setDrive(0,0);
	setClawTo(500);
	setLiftTo(600);
	setClawTo(320);
	setLiftTo(1000);
	setClawTo(500);
	wait1Msec(1000);
	liftFullDown();
}

task autonomous()//should actually be auto
{
	startTask(autoClaw);
	startTask(autoLift);
	startTask(clawDump);
	clearAllSensors();
	fenceStarAuto();
}

task usercontrol()//actual user
{
	startTask(clawDump);
	lastClawPos = SensorValue[clawPot];
	while (true)
	{
		int ry = vexRT[Ch2];
		int ly = vexRT[Ch3];
		int rx = vexRT[Ch1];
		int lx = vexRT[Ch4];
		setDrive(ry, ly);//tank
		//setDrive(ry-rx, ry+rx);//arcade
		//setDrive(ry-lx, ry+lx);//split arcade
		int up = vexRT[Btn5U]>0?1:0;
		int down = vexRT[Btn5D]>0?1:0;
		int clawIn = vexRT[Btn6U];
		int clawOut = vexRT[Btn6D];
		int goToDown = vexRT[Btn8D];
		int clawClose = vexRT[Btn7L];
		int	lift = up+down==2?0:up==1?1:down==1?-1:0;
		if(goToDown)
		{
			lastLiftPos = -100;
			lastClawPos = 1000;
			lastClawPower = 0;
		}
		else
		{
			liftMaster(lift*127);
		}
		int claw = clawOut+clawIn==2?0:clawOut==1?1:clawIn==1?-1:0;
		if (clawClose)
		{
			lastClawPos = 400;
			lastClawPower = 0;
		}
		else
		{
			clawMaster(claw*127);
		}
		wait1Msec(20);
	}
}
/*

task recordAutoCode()//actually recordAutoCode
{
writeDebugStreamLine("//Begin recording in: 3");
wait1Msec(1000);
writeDebugStreamLine("//Begin recording in: 2");
wait1Msec(1000);
writeDebugStreamLine("//Begin recording in: 1");
wait1Msec(1000);

startTask (recordVexRT);
writeDebugStreamLine("//BEGIN AUTON RECORDING");

writeDebugStreamLine("// Copy the following lines into your autonomous task:");

clearTimer(T1);
while (time1[T1] < 15000)
{
int ry = vexRT[Ch2];
int ly = vexRT[Ch3];
setDrive(ry, ly);
int up = vexRT[Btn5U]>0?1:0;
int down = vexRT[Btn5D]>0?1:0;
int clawOut = vexRT[Btn6U];
int clawIn = vexRT[Btn6D];
int hangUp = vexRT[Btn8R];
int hangDown = vexRT[Btn8D];
int	lift = up+down==2?0:up==1?1:down==1?-1:0;
setLift(lift*127);
int claw = clawOut+clawIn==2?0:clawOut==1?1:clawIn==1?-1:20.0/127.0;
setClaw(claw*127);
wait1Msec(50);
}

stopTask (recordVexRT);
writeDebugStreamLine("//END AUTON RECORDING");

// stop all motors
motor[port1] = motor[port2] = motor[port3] = motor[port4] = motor[port5] = motor[port6] = motor[port7] = motor[port8] = motor[port9] = motor[port10] = 0;
*/
