/*----------------------------------------------------------------------------------------------
파일명 : THL_sensor.h
기 능  : gpio 센서 클래스헤더

----------------------------------------------------------------------------------------------*/
#ifndef THL_SENSOR_H_
#define THL_SENSOR_H_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <time.h>
#define TYPE_INIT 0
#define DATA_INIT 0
#define VOLT_INIT 0

class Sensor{
private:
	int sensorType;// 1: Temp 2: Humid 3:Light
	int mcpData;
	float volt;
	float realData;
	bool timerOn;
	int electronicOnTime;
	int pin;
	unsigned char spiChannel;
	int pir; 
	clock_t startTime;
public:
	Sensor(int pn, unsigned char channel, int pir);
	Sensor(int pn, unsigned char channel);
	int Detection();
	int MotionDetection();
	int SensorSetting();//SetPi(),SetSpi()
	int SetPi();
	int SetSpi();
	//get,set field
	void SetSensorType(int type);
	int GetMcpData();
	float GetVolt();
	void SetVolt(float vol);
	float GetRealData();
	void SetRealData(float realValue);
	bool GetTimerOn();
	void SetTimerOn(bool on);
	int GetElectronicOnTime();
	clock_t GetStartTime();
	//actuate electronic
	void RelaySetting();
	void ElectronicOn();
	void ElectronicOff();

	int ReadMcpData();
	int ReadMotionData();
	float McpDataToVolt();
	virtual float VoltToRealData() = 0;
	void ShowData();
};
#endif