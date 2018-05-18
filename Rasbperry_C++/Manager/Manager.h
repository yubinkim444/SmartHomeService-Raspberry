/*----------------------------------------------------------------------------------------------
파일명 : TCP_server.h
기 능 : 서버전용 헤더
----------------------------------------------------------------------------------------------*/
#ifndef MANAGER_H_
#define MANAGER_H_

#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "LinkedList.h"

#include <time.h>
#include "/usr/include/mysql/mysql.h"
#define BUF_LEN 128 
#define TEMP 1
#define HUMD 2
#define LUX 3
#define MOTION 4
#define TCP_PORT 8080
#define SPRNG_AUTUM 1
#define SUMMER 2
#define WINTER 3
//-------------------ServerCommuni-----------
#define DBHOST "220.76.187.135"
#define DBUSER "root"
#define DBPASS "dbqls1004"
#define DBNAME "SmartHomeServer"
extern pthread_mutex_t mutexLock;
class Manager{
private:
	//socket ConnectSetting
	struct sockaddr_in serverAddr, clientAddr;
	int on;
	int serverFd, clientFd; 
	socklen_t len, lenOut;
	int port; 
	//---------------------SensorManager------------------
	SensorManager* sensorManager;
	char clientName[BUFSIZ];
	char myName[BUFSIZ];
	//-----------------------------------------------
	//communication with server
	//mysql
	MYSQL connector;
	MYSQL_RES* sqlResult;
	MYSQL_ROW sqlRow;
	char query[BUFSIZ];
	int myCode;
   //-------------------------------------------------------
	//sensor data msg
	char emptyTempMsg[BUF_LEN + 1];
	char emptyHumdMsg[BUF_LEN + 1];
	char emptyLuxMsg[BUF_LEN + 1];
	char emptyMotionMsg[BUF_LEN + 1];
	//sensor Table
	char bufIpz[BUFSIZ];
	//------------------------------------------------------
	char buf[BUF_LEN + 1];
	//optimum data
	int weather;
	float optimumTemp;
	float optimumHumd;
	float optimumLux;
	float optimumMotion;
	char optimumTempMsg[BUF_LEN + 1];
	char optimumHumdMsg[BUF_LEN + 1];
	char optimumLuxMsg[BUF_LEN + 1];
	char optimumMotionMsg[BUF_LEN + 1];
	
public:
	Manager();
	Manager(int port);
	void ConnectSetting();
	//void SetMsg(char Msg[], int n);
	void SetOptMsg(char Msg[], int n);
	//thread function
	void* Connect(void *data);
	static void* CallBackConnect(void* server_ptr);

    //get,ConnectSetting field(socket network)
	struct sockaddr_in GetServerAddr();
	struct sockaddr_in GetClientAddr();
	int GetServerFd();
	int GetClientFd();
	socklen_t GetClientLen();
	void SetClientLen(sockaddr_in client);
	//get,ConnectSetting field(optimum data)
	int GetWeather();
	float GetOptimumTemp();
	float GetOptimumHumd();
	float GetOptimumLux();
	float GetOptimumMotion();

	void SetWeather(int weather);
	void SetOptimumTemp(float temp);
	void SetOptimumHumd(float humd);
	void SetOptimumLux(float lux);
	void SetOptimumMotion(float motion);
	//print msg
	void ShowMsg();
	//ConnectSetting optimum data with weather
	void SetOptimumDataWithWeather(int month);
	void SetOptimumDataToString();
	void SetOptimumStringToData();

	void ManageMsg(char buf[]);
	char* MsgCutForGetSendIpAddr(char buf[]);
	void ReceiveSensorMsg(char buf[]);
	void SendSensorMsg(char buf[]);
	void ReceiveOptimumMsg(char buf[]);
	void SendOptimumMsg(char buf[]);
	//-----------------------------------------------------------------------
	void CommuniSetting();
	void SendCommuniMsg();
	static void* CallBackCommuniConnect(void* server_ptr);
	void WriteQuery();

	void ManagerSetQuery();
	void WriteAllData(int hour);
	float MsgTransformToQuery(char msg[]);
	~Manager();
};

#endif