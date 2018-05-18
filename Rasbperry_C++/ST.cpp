/*----------------------------------------------------------------------------------------------
���ϸ� : ST.cpp
�� �� : ����Ʈ �µ� ���� ���� �������� ����
������ : g++ -o ST ST.cpp THL_sensor.h THL_sensor.cpp -lwiringPi
���࿹ : sudo ./ST
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

