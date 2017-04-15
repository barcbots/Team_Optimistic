#pragma config(UART_Usage, UART1, uartUserControl, baudRate115200, IOPins, None, None)
#pragma config(Sensor, in1,    intakePot,      sensorPotentiometer)
#pragma config(Sensor, in2,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, dgtl1,  rightQuad,      sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  leftQuad,       sensorQuadEncoder)
#pragma config(Sensor, dgtl10, stopBtn,        sensorTouch)
#pragma config(Sensor, dgtl11, testBtn,        sensorTouch)
#pragma config(Sensor, dgtl12, liftStopButton, sensorTouch)
#pragma config(Motor,  port1,           liftRO,        tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           intakeY,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           liftLI,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           driveLB,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           driveLFY,      tmotorVex393_MC29, openLoop, encoderPort, dgtl6)
#pragma config(Motor,  port6,           driveRFY,      tmotorVex393_MC29, openLoop, encoderPort, dgtl1)
#pragma config(Motor,  port7,           driveRB,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           liftRI,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           liftLO,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          lidar,         tmotorVex393_HBridge, openLoop, reversed, encoderPort, None)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma DebuggerWindows("debugStream")
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define BCI_SEM_DEBUG_FULL_OFF
#define BCI_USE_UART
#define BCI_USE_TIMER
#define BCI_USE_POS_PID
#define BCI_USE_VEL_PID
#include "BCI\BCI.h"

//#define UARTHANDLER_DEBUG
//#define UARTHANDLER_DEBUG_READ
//#define MOVETOPOINT_DEBUG
//#define POINTMATH_DEBUG
//#define MPC_DEBUG

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
#include "noWarning.c"

task monitorStop()
{
	while (true)
	{
		if (SensorValue[stopBtn])
		{
			stopAllTasks();
			for (int i = 0; i < 10; i++)
			{
				motor[i] = 0;
			}
		}

		EndTimeSlice();
	}
}

task main()
{
	startTask(monitorStop);

	clearDebugStream();

	motor[lidar] = 55; //64 at 7.51V, 48 at 8.27V

	initUART();
	initSensors();

	//startTask(testDrive);
	//startTask(testLift);
	intakeAndLiftTask_intakeState = INTAKE_REST;
	intakeAndLiftTask_liftState = LIFT_REST;
	startTask(intakeAndLiftTask);
	startTask(readBuffer);
	wait1Msec(250);

	while(!vexRT[Btn8D]){wait1Msec(15);}

	driveStraight_Ballsy(-550);
	intakeAndLiftTask_liftState = LIFT_DOWN;
	intakeAndLiftTask_intakeState = INTAKE_OPEN;
	pickUp(3250,350,true);


	intakeAndLiftTask_intakeState = INTAKE_CLOSED;
	intakeAndLiftTask_liftState = LIFT_DOWN;
	wait1Msec(650);
	driveStraight_Ballsy(-400);
	intakeAndLiftTask_liftState = LIFT_HALF;
	dumpIntake();
	startTask(commandRobot);

	while (true) { wait1Msec(15); }
}
