/*----------------------------------------------------------------------------------------------
파일명 : TCP_client.h
기 능  : 통신 클라이언트 클래스헤더

----------------------------------------------------------------------------------------------*/
#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#define TCP_PORT 5100
#define TEMP 1
#define HUMD 2
#define LUX 3
#define MOTION 4
/**************Client class area******************/
class Client {
private:
	int sock;
	int on;
	socklen_t serverAddrLen;
	//adress structor
	char ip[BUFSIZ];
	struct sockaddr_in serverAddr;
	//SendMyData to string
	char communicationOptimumMsgToSend[BUFSIZ];
	char communicationSensorMsgToSend[BUFSIZ];
	char tempMsg[BUFSIZ + 1];
	char humdMsg[BUFSIZ + 1];
	char luxMsg[BUFSIZ + 1];
	char motionMsg[BUFSIZ + 1];
public:
	Client();
	//get field
	int GetSockAddr();
	struct sockaddr* GetServerAddr();
	socklen_t GetServerAddrLen();
	char* GetCommunicationSensorMsgToSend();
	char* GetCommunicationOptimumMsgToSend();
	char* GetTempMsg();
	char* GetHumdMsg();
	char* GetLuxMsg();
	char* GetMotionMsg();
	//set field
	void SetSockAddr(int);
	void SetCommunicationOptimumMsgToSend(char[]);
	void SetCommunicationSensorMsgToSend(char[]);
	void SetTempMsg(char[]);
	void SetHumdMsg(char[]);
	void SetLuxMsg(char[]);
	void SetMotionMsg(char[]);
	char* GetIp();
	void SetIp(char []);
	void ConnectionSetting(char* argv);
	void MsgSetting(char Data[], int n);

	void PrintMsg()const;
	~Client();
};
#endif