/*----------------------------------------------------------------------------------------------
파일명 : SmartSensor.h
기 능  : 스마트 센서 클래스헤더

----------------------------------------------------------------------------------------------*/
#ifndef SMARTSENSOR_H_
#define SMARTSENSOR_H_
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include "TCPClient.h"
#include "Sensor.h"
extern pthread_mutex_t mutexLock;
/**************SmartSensor(Temp,Humd,Lux)class  area******************/
class SmartSensor :public Sensor, public Client{
private:
	
	char optimumMsg[BUFSIZ];
	float optimumData;
	float tolerance;
public:
	SmartSensor(int pin, unsigned char channel, int pir);
	SmartSensor(int pin, unsigned char channel);
        ~SmartSensor();
	//set SendMyData for ReceiveOtherData msg(RT,RH,RL)
	void CommunicationMsgSetting(int type);
	//transform char optimumData to int data
	void OptimumMsgToData();
	void Sleep();
	void ReceiveOptimumMsg();
	//get,set field
	char* GetOptimumMsg();
	void SetOptimumMsg(char optimum[]);
	float GetOptimumData();
	void SetOptimumData(float optData);
	float GetTolerance();
	void SetTolerance(float tol);
	//virtual(interface)
	virtual float VoltToRealData() = 0;

	virtual void SendMsgSetting(float data)=0;
	virtual void SendMyData()=0;
	//virtual void ReceiveOtherData(int type)=0;
	
	virtual float MyRealData()=0;
	virtual void Decision()=0;
	virtual void CalculateDecision() = 0;
	void Start(void* data);
	static void* CallBackStart(void* ptr);
};
class TempSmartSensor :public SmartSensor{
public:
	TempSmartSensor(int pin,unsigned char channel);
	//virtual function make
	virtual float VoltToRealData();
	virtual void SendMsgSetting(float data);
	virtual void SendMyData();
	//virtual void ReceiveOtherData(int type);

	virtual float MyRealData();
	virtual void Decision();
	virtual void CalculateDecision();
};
class HumdSmartSensor :public SmartSensor{
public:
	HumdSmartSensor(int pin,unsigned char channel);
	//virtual function make
	virtual float VoltToRealData();
	virtual void SendMsgSetting(float data);
	virtual void SendMyData();
	//virtual void ReceiveOtherData(int type);
	
	virtual float MyRealData();
	virtual void Decision();
	virtual void CalculateDecision();
};
class LuxSmartSensor :public SmartSensor{
public:
	LuxSmartSensor(int pin,unsigned char channel);
	//virtual function make
	virtual float VoltToRealData();
	virtual void SendMsgSetting(float data);
	virtual void SendMyData();
	//virtual void ReceiveOtherData(int type);
	
	virtual float MyRealData();
	virtual void Decision();
	virtual void CalculateDecision();
};
class MotionSmartSensor :public SmartSensor{
public:
	MotionSmartSensor(int pin,unsigned char channel,int pir);
	//virtual function make
	virtual float VoltToRealData();
	virtual void SendMsgSetting(float data);
	virtual void SendMyData();
	//virtual void ReceiveOtherData(int type);

	virtual float MyRealData();
	virtual void Decision();
	virtual void CalculateDecision();
};
#endif