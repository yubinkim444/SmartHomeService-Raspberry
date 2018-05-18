/*----------------------------------------------------------------------------------------------
파일명 : Server.cpp
기 능 : 서버전용 함수 구현
----------------------------------------------------------------------------------------------*/
#include "Server.h"
#include <ctime>
#define BUF_LEN 128 
#define TEMP 1
#define HUMD 2
#define LUX 3
#define MOTION 4

#define SPRNG_AUTUM 1
#define SUMMER 2
#define WINTER 3

//Server constructor
Server::Server() {
	perror("port number please");
	exit(1);
}
Server::Server(int port) :port(port) {

	connector = mysql_init(NULL);
	if (!mysql_real_connect(connector, DBHOST, DBUSER, DBPASS, DBNAME, 3306, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(connector));
		return;
	}
	printf("database open...\n");

}
//Server tcp network socket setting
void Server::ConnectSetting() {
	/* 소켓 생성 */

	if ((serverFd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Server: Can't open stream socket.");
		exit(1);
	}
	/* serverAddr을 '\0'으로 초기화 */
	on = 1;
	setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	bzero((char *)&serverAddr, sizeof(serverAddr));

	/* serverAddr 세팅 */
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);

	/* bind() 호출 */
	if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		printf("Server: Can't bind local address.\n");
		exit(1);
	}
	/* 소켓을 수동 대기모드로 세팅 */
	listen(serverFd, 5);

}
//for thread function
void* Server::Connect(void *data) {
	pthread_t thread_id = pthread_self();
	while (1) {
		pthread_mutex_lock(&mutexLock);
		printf("Server : waiting connection request.\n");
		len = sizeof(clientAddr);
		clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &len);
		if (clientFd < 0)
		{
			printf("Server: accept failed.\n");
			exit(1);
		}
		else {

			strcpy(clientName, inet_ntoa(clientAddr.sin_addr));
			StoreClientInform(clientName);
			//get client information
			printf("Server : client connected.\n");
			memset(buf, 0, sizeof(buf));
			lenOut = read(clientFd, buf, sizeof(buf));
			UpdateDataBase(buf);
			close(clientFd);
		}
		pthread_mutex_unlock(&mutexLock);
	}

}
// call back function for thread
void* Server::CallBackConnect(void* server_ptr) {
	Server* s = (Server*)server_ptr;
	s->Connect(0);
}
// msg setting 

//get, ConnectSetting socket field
struct sockaddr_in Server::GetServerAddr() {
	return serverAddr;
}
struct sockaddr_in Server::GetClientAddr() {
	return clientAddr;
}
int Server::GetServerFd() {
	return serverFd;
}
int Server::GetClientFd() {
	return clientFd;
}
void Server::SetClientLen(sockaddr_in client) {
	len = sizeof(client);
}
socklen_t Server::GetClientLen() {
	return len;
}
void  Server::UpdateDataBase(char buf[]) {
	CutMsg(buf);
	switch (buf[0]) {
	case 'S':
		StoreSensMsg(buf);
		break;
	case 'U':
		StoreOptMsg(buf);
		break;
	default:
		printf("read error");
		return;
	}
}
void Server::StoreSensMsg(char buf[]) {
	switch (buf[1]) {
	case 'T':  printf("get: %s, client:%s\n", buf, clientName);
		SetTempData();
		break;
	case 'H':  printf("get: %s, client:%s\n", buf, clientName);
		SetHumdData();

		break;
	case 'L': printf("get: %s, client:%s\n", buf, clientName);
		SetLuxData();

		break;
	case 'M': printf("get: %s, client:%s\n", buf, clientName);
		SetMotionData();

		break;
	default: printf("store error\n"); return;
	}
}
void Server::StoreOptMsg(char buf[]) {
	switch (buf[1]) {
	case 'T': printf("get: %s, client:%s\n", buf, clientName);
		SetTempOptData();
		break;
	case 'H':  printf("get: %s, client:%s\n", buf, clientName);
		SetHumdOptData();
		break;
	case 'L':  printf("get: %s, client:%s\n", buf, clientName);
		SetLuxOptData();
		break;
	case 'M':  printf("get: %s, client:%s\n", buf, clientName);
		SetMotionOptData();
		break;
	default: printf("store opt error\n"); return;
	}
}
void Server::StoreClientInform(char clientName[]) {
	printf("client connect: %s\n", clientName);
	if (CheckClient()) {
		sprintf(query, "insert into home values ('%s', now())", clientName);
		WriteQuery();
		sprintf(query, "insert into homeStatus values ('%s', false, false, false, false)", clientName);
		WriteQuery();
	}
}
void  Server::CutMsg(char buf[]) {
	int i;
	for (i = 2; i < sizeof(buf); i++) {
		dataBuf[i - 2] = buf[i];
	}
}
void  Server::SetTempData() {
	float bufs;
	if (dataBuf[0] != 'N') {
		sscanf(dataBuf, "%f.2", &bufs);
		UpdateTempHomeStatus();
		sprintf(query, "insert into dataValue values (%d,'%s', %f, now())", TEMP, clientName, bufs);
		WriteQuery();
	}
}
void  Server::SetHumdData() {
	float bufs;
	if (dataBuf[0] != 'N') {
		sscanf(dataBuf, "%f.2", &bufs);
		UpdateHumdHomeStatus();
		sprintf(query, "insert into dataValue values (%d, '%s', %f, now())", HUMD, clientName, bufs);
		WriteQuery();
	}
}
void  Server::SetLuxData() {
	float bufs;

	if (dataBuf[0] != 'N') {
		sscanf(dataBuf, "%f.2", &bufs);
		UpdateLuxHomeStatus();
		sprintf(query, "insert into dataValue values (%d, '%s', %f, now())", LUX, clientName, bufs);
		WriteQuery();
	}
}
void  Server::SetMotionData() {
	float bufs;

	if (dataBuf[0] != 'N') {
		sscanf(dataBuf, "%f.2", &bufs);
		UpdateMotionHomeStatus();
		sprintf(query, "insert into dataValue values (%d, '%s', %f, now())", MOTION, clientName, bufs);
		WriteQuery();
	}
}

void  Server::SetTempOptData() {
	float bufs;
	sscanf(dataBuf, "%f.2", &bufs);
	sprintf(query, "insert into optValue values (%d, '%s', %f, now())", TEMP, clientName, bufs);
	WriteQuery();
}
void  Server::SetHumdOptData() {
	float bufs;
	sscanf(dataBuf, "%f.2", &bufs);
	sprintf(query, "insert into optValue values (%d, '%s', %f, now())", HUMD, clientName, bufs);
	WriteQuery();
}
void  Server::SetLuxOptData() {
	float bufs;
	sscanf(dataBuf, "%f.2", &bufs);
	sprintf(query, "insert into optValue values (%d, '%s', %f, now())", LUX, clientName, bufs);
	WriteQuery();
}
void  Server::SetMotionOptData() {
	float bufs;
	sscanf(dataBuf, "%f.2", &bufs);
	sprintf(query, "insert into optValue values (%d, '%s', %f, now())", MOTION, clientName, bufs);
	WriteQuery();
}
void Server::UpdateTempHomeStatus() {
	sprintf(query, "update homeStatus set temp = true where homeIp = '%s'", clientName);
	WriteQuery();
}
void Server::UpdateHumdHomeStatus() {
	sprintf(query, "update homeStatus set humd = true where homeIp = '%s'", clientName);
	WriteQuery();
}
void Server::UpdateLuxHomeStatus() {
	sprintf(query, "update homeStatus set lux = true where homeIp = '%s'", clientName);
	WriteQuery();
}
void Server::UpdateMotionHomeStatus() {
	sprintf(query, "update homeStatus set motion = true where homeIp = '%s'", clientName);
	WriteQuery();
}
void Server::WriteQuery() {
	int res;
	if (res = mysql_query(connector, query)) {
		fprintf(stderr, "%s\n", mysql_error(connector));
		printf("write db error\n");
	}
	else { printf("write success\n"); }
	memset(query, 0, sizeof(query));
}
bool Server::CheckClient() {
	sprintf(query, "select homeIp from home where homeIp = '%s'", clientName);
	WriteQuery();
	sqlResult = mysql_use_result(connector);
	if ((sqlRow = mysql_fetch_row(sqlResult)) == NULL) {
		mysql_free_result(sqlResult);
		return true;
	}
	else {
		mysql_free_result(sqlResult);
		return false;
	}
}
//Server destructor 
Server::~Server() {
	close(serverFd);
	mysql_close(connector);
}

