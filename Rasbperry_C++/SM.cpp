/*----------------------------------------------------------------------------------------------
���ϸ� : SM.cpp
�� �� : ����Ʈ �µ� ���� ���� �������� ����
������ :
���࿹ : sudo ./SM
----------------------------------------------------------------------------------------------*/
//THL sensor header
#include "Sensor.h"
//Client header
#include "TCPClient.h"
//Smart Sensor header
#include "SmartSensor.h"

int main(void){
	MotionSmartSensor motionSensor(7,4);
	bool sendOrReceive = true;
	motionSensor.ReceiveOptimumMsg();
	while (1){
		if (sendOrReceive){
			motionSensor.SendMyData();
			motionSensor.Decision();
			sendOrReceive = false;
		}
		else{
			motionSensor.ReceiveOptimumMsg();
			sendOrReceive = true;
		}
		sleep(1);
		motionSensor.PrintMsg();
	}
	return 0;
}

