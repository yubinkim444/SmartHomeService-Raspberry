/*----------------------------------------------------------------------------------------------
���ϸ� : SH.cpp
�� �� : ����Ʈ ���� ���� ���� �������� ����
������ : g++ -o SH SH.cpp THL_sensor.h THL_sensor.cpp -lwiringPi
���࿹ : sudo ./ST
----------------------------------------------------------------------------------------------*/
//THL sensor header
#include "Sensor.h"
//Client header
#include "TCPClient.h"
//Smart Sensor header
#include "SmartSensor.h"

int main(void){
	HumdSmartSensor humdSensor(7,0);
	bool sendOrReceive = true;
	humdSensor.ReceiveOptimumMsg();
	while (1){
		if (sendOrReceive){
			humdSensor.SendMyData();
			humdSensor.Decision();
			sendOrReceive = false;
		}
		else{
			humdSensor.ReceiveOptimumMsg();
			sendOrReceive = true;
		}
		sleep(1);
		humdSensor.PrintMsg();
	}

	return 0;
}

