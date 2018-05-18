/*----------------------------------------------------------------------------------------------
파일명 : ST.cpp
기 능 : 스마트 온도 센서 정보 서버에게 전송
컴파일 : g++ -o ST ST.cpp THL_sensor.h THL_sensor.cpp -lwiringPi
실행예 : sudo ./ST
----------------------------------------------------------------------------------------------*/
//THL sensor header
#include "Sensor.h"
//Client header
#include "TCPClient.h"
//Smart Sensor header
#include "SmartSensor.h"

int main(void){
	TempSmartSensor tempSensor(7,0);
	HumdSmartSensor humdSensor(2,1);
	pthread_t tempId;
	pthread_t humdId;
	pthread_create(&tempId,NULL, SmartSensor::CallBackStart,&tempSensor);
	pthread_create(&humdId,NULL, SmartSensor::CallBackStart,&humdSensor);
	
	pthread_join(tempId,NULL);
	pthread_join(humdId,NULL);
       
	return 0;
}

