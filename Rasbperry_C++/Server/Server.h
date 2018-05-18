#ifndef SERVER_H_
#define SERVER_H_

#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <mysql/mysql.h>
#include <arpa/inet.h>

#define BUF_LEN 128 
#define TEMP 1
#define HUMD 2
#define LUX 3
#define DBHOST "127.0.0.1"
#define DBUSER "root"
#define DBPASS "dbqls1004"
#define DBNAME "smartsensor"
extern pthread_mutex_t mutexLock;
class Server{
private:
	//socket ConnectSetting
	struct sockaddr_in serverAddr, clientAddr;
	int on;
	int serverFd, clientFd;
	socklen_t len, lenOut;
	int port;
	char buf[BUFSIZ];
	//recent data
	char dataBuf[BUFSIZ];
	char clientName[BUFSIZ];
	//Mysql
	MYSQL* connector;
	MYSQL_RES* sqlResult;
	MYSQL_ROW sqlRow;
	char query[1024];

public:
	Server();
	Server(int port);
	void ConnectSetting();
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
	void UpdateDataBase(char buf[]);
	void StoreOptMsg(char buf[]);
	void StoreSensMsg(char buf[]);
	void StoreClientInform(char clientName[]);
	bool CheckClient();
	void CutMsg(char buf[]);
	void SetTempData();
	void SetHumdData();
	void SetLuxData();
	void SetMotionData();
	void SetTempOptData();
	void SetHumdOptData();
	void SetLuxOptData();
	void SetMotionOptData();
	void WriteQuery();
	void UpdateTempHomeStatus();
	void UpdateHumdHomeStatus();
	void UpdateLuxHomeStatus();
	void UpdateMotionHomeStatus();
	~Server();

};


#endif
