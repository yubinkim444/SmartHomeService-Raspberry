/*----------------------------------------------------------------------------------------------
���ϸ� : SL.cpp
�� �� : ����Ʈ ���� ���� ���� �������� ����
������ : g++ -o SL SL.cpp THL_sensor.h THL_sensor.cpp -lwiringPi
���࿹ : sudo ./SL
----------------------------------------------------------------------------------------------*/
//THL sensor header
#include "Sensor.h"
//Client header
#include "TCPClient.h"
//Smart Sensor header
#include "SmartSensor.h"

int main(void){
	LuxSmartSensor luxSensor(7,0);
	bool sendOrReceive = true;
	luxSensor.ReceiveOptimumMsg();
	while (1){
		if (sendOrReceive){
			luxSensor.SendMyData();
			luxSensor.Decision();
			sendOrReceive = false;
		}
		else{
			luxSensor.ReceiveOptimumMsg();
			sendOrReceive = true;
		}
		sleep(1);
		luxSensor.PrintMsg();
	}

	return 0;
}

