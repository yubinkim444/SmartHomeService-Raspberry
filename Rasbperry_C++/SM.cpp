/*----------------------------------------------------------------------------------------------
파일명 : SM.cpp
기 능 : 스마트 온도 센서 정보 서버에게 전송
컴파일 :
실행예 : sudo ./SM
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

