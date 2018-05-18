#ifndef SENSORMANAGER_H_
#define SENSORMANAGER_H_
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include<time.h>
#pragma warning (disable:4996)
#define FALSE 0
#define TRUE 1
class SmartThing {
public:
	char* ip;
	int sensorType;
	char* msg;
	char* opt;
	SmartThing* link;
	int timer;
public:
	SmartThing(char ip[], int sensorType, char msg[],char opt[]);
	void setOptMsg(char buf[]);
	float getOptVal();
	void setMsg(char buf[]);
	~SmartThing();
};
class SensorManager {
private:
	SmartThing **phead;
	SmartThing *head; // 헤드 포인터
	int length;
	int tempLen;
	int humdLen;
	int luxLen;
	int motionLen;
public:
	SensorManager();
	void insertSmartThing(SmartThing *p, SmartThing *new_node);
	void removeSmartThing(SmartThing *p, SmartThing *removed);
	int getLength();
	void error(char *message);
	void add(int position, SmartThing* data);
	void addLast(SmartThing* data);
	void addFirst(SmartThing* data);
	int isEmpty();
	void deleteSmartThing(int pos);
	SmartThing* getEntry(int pos);
	void clear();
	void display();
	SmartThing * isInList(int type, char ips[]);
	SmartThing * getSmartThingAt(int pos);

	void AddTempLen();
	void AddHumdLen();
	void AddLuxLen();
	void AddMotionLen();
	
	int GetTempLen();
	int GetHumdLen();
	int GetLuxLen();
	int GetMotionLen();

	void SubTempLen();
	void SubHumdLen();
	void SubLuxLen();
	void SubMotionLen();

	void SensorErrorCheck();
};

#endif