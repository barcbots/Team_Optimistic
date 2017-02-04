#pragma config(UART_Usage, UART1, uartUserControl, baudRate115200, IOPins, None, None)
#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    intakePot,      sensorPotentiometer)
#pragma config(Sensor, dgtl1,  rightQuad,      sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  leftQuad,       sensorQuadEncoder)
#pragma config(Sensor, dgtl12, liftStopButton, sensorTouch)
#pragma config(Sensor, I2C_1,  liftIME,        sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           lidar,         tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           liftRO,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           liftLI,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           driveLFY,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           driveRB,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           driveLB,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           driveRFY,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           intakeY,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           liftLO,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          liftRI,        tmotorVex393_HBridge, openLoop, encoderPort, I2C_1)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define BCI_SEM_DEBUG_FULL_OFF
#define BCI_USE_LCDCONTROL
#define MENU_NUM 1
//#define LCD_NO_CALLBACKS
#define LCD_NO_SAFETY
//#define BCI_USE_HEAP
#define BCI_USE_UART
#define BCI_USE_TIMER
#define BCI_USE_POS_PID
#define BCI_USE_VEL_PID
#include "BCI\BCI.h"

//#define UARTHANDLER_DEBUG
#define UARTHANDLER_DEBUG_READ
#define MOVETOPOINT_DEBUG
#define POINTMATH_DEBUG

#include "uartHandler.c"
#include "pointMath.c"
#include "basicMotorControl.c"
#include "drivingFunctions.c"
#include "turningFunctions.c"
#include "intakeAndLiftHandler.c"
#include "collisionHandler.c"
#include "motorControl.c"
#include "decisionHandler.c"
#include "skills.c"

task main()
{
	clearDebugStream();

	motor[lidar] = 80; //64 at 7.51V, 48 at 8.27V

	//string voltageString = "";
	//menu *voltage = newMenu(voltageString);
	string rpmString = "";
	menu *rpm = lcd_newMenu(rpmString, 1);
	startTask(lcdControlTask);
	timer lcdTimer;
	timer_Initialize(&lcdTimer);

	initUART();
	initSensors();

	//Start reading from pi
	startTask(readBuffer);

	bool isUserControlled = false, isUserControlled_last = true;
	int leftVal, rightVal;

	while (true)
	{
		//Send data to pi
		sendSTDMsg();

		if (timer_Repeat(&lcdTimer, 250))
		{
			long lidarRPM = 0;

			BCI_lockSem(std_msgSem, "main")
			{
				lidarRPM = std_msg[STD_MSG_LIDAR_RPM];
				BCI_unlockSem(std_msgSem, "main")
			}

			sprintf(rpmString, "RPM: %d, M: %d", lidarRPM, motor[lidar]);
			lcd_changeMessage(rpm, rpmString);
		}

		//Runs skills
		if (vexRT[JOY_BTN_LU])
		{
			runSkills();
			waitForZero(vexRT[JOY_BTN_LU]);
		}

		//Switch between driver control and autonomous
		if (vexRT[JOY_BTN_RL])
		{
			isUserControlled = !isUserControlled;
			waitForZero(vexRT[JOY_BTN_RL]);
		}

		if (isUserControlled)
		{
			leftVal = vexRT[JOY_JOY_LV];
			rightVal = vexRT[JOY_JOY_RV];
			leftVal = abs(leftVal) < JOY_THRESHOLD ? 0 : leftVal;
			rightVal = abs(rightVal) < JOY_THRESHOLD ? 0 : rightVal;

			setLeftMotors(leftVal);
			setRightMotors(rightVal);

			setIntakeMotors(127 * vexRT[JOY_TRIG_LU] + -127 * vexRT[JOY_TRIG_LD]);
			setLiftMotors(127 * vexRT[JOY_TRIG_RU] + -127 * vexRT[JOY_TRIG_RD]);
		}
		else
		{
			if (isUserControlled != isUserControlled_last)
			{
				startTask(commandRobot);
			}
		}

		isUserControlled_last = isUserControlled;

		if (vexRT[JOY_BTN_RU])
		{
			motor[lidar] += 1;
			waitForZero(vexRT[JOY_BTN_RU]);
		}
		else if (vexRT[JOY_BTN_RD])
		{
			motor[lidar] -= 1;
			waitForZero(vexRT[JOY_BTN_RD]);
		}

		//Task wait
		wait1Msec(15);
	}
}

void lcd_invoke(int func)
{
	switch (func)
	{
		case 1:
			motor[lidar] -= 10;
			break;

		default:
			break;
	}
}
