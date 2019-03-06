// Created by H.Levine with Atom Array team, Lukin group (2016).


#include <iostream>
#include <chrono>
#include <thread>
#include <libadwin.h>
#include <libadwin/errno.h>


static const short DEVICE_ID = 0x1;
//static const short RECV_CAMERA_CONFIG_PAR_INDEX = 72;
//static const short RECV_CONFIG_PAR_INDEX = 73;
static const short RECV_TRIGGER_PAR_INDEX = 75;
static const short SEND_TRIGGER_PAR_INDEX = 77;

static const short START_SEQUENCE_TRIGGER_VAL = 1;
static const short ANALYZE_IMAGE_TRIGGER_VAL = 2;

using namespace std;

void initializeAdwinConnection() {
	Set_DeviceNo(DEVICE_ID);
}

long sendTrigger(void)
{
	// send a trigger to system control
	Set_Par(SEND_TRIGGER_PAR_INDEX, 1);

	int32_t error;
	error = Get_Last_Error();
	return error;
}

long resetSendTrigger(void)
{
	Set_Par(SEND_TRIGGER_PAR_INDEX, 0);

	int32_t error;
	error = Get_Last_Error();
	return error;
}

long resetRecvTrigger(void)
{
	// reset our receive trigger
	long error = 0l;
	Set_Par(RECV_TRIGGER_PAR_INDEX, 0);
	return error;
}


long waitForStartSequenceTrigger(void)
{
	// wait for the trigger from system control
	long error = 0l;
	while (Get_Par(RECV_TRIGGER_PAR_INDEX) != START_SEQUENCE_TRIGGER_VAL)
	{
		this_thread::sleep_for(chrono::milliseconds(2));
	}
	return error;
}

long waitForAnalyzeImageTrigger(void)
{
	long error = 0l;
	
	while (Get_Par(RECV_TRIGGER_PAR_INDEX) != ANALYZE_IMAGE_TRIGGER_VAL)
	{
		this_thread::sleep_for(chrono::milliseconds(2));
	}
	return error;
}