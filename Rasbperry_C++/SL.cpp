/*----------------------------------------------------------------------------------------------
파일명 : SL.cpp
기 능 : 스마트 조도 센서 정보 서버에게 전송
컴파일 : g++ -o SL SL.cpp THL_sensor.h THL_sensor.cpp -lwiringPi
실행예 : sudo ./SL
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

