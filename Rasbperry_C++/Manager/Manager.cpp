/*----------------------------------------------------------------------------------------------
파일명 : TCP_server.cpp
기 능 : 서버전용 함수 구현
----------------------------------------------------------------------------------------------*/
#include "Manager.h"
#include <ctime>
#define BUF_LEN 128 
#define TEMP 1
#define HUMD 2
#define LUX 3
#define MOTION 4

#define SPRNG_AUTUM 1
#define SUMMER 2
#define WINTER 3

//Manager constructor
Manager::Manager(){
	perror("port number please");
	exit(1);
}
Manager::Manager(int port) :port(port){
	strcpy(emptyTempMsg, "STN");
	strcpy(emptyHumdMsg, "SHN");
	strcpy(emptyLuxMsg, "SLN");
	strcpy(emptyMotionMsg, "SMN");
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	int month = now->tm_mon + 1;
	sensorManager = new  SensorManager();
	//init optimum data
	SetOptimumDataWithWeather(month);
	printf("month: %d\n", month);
	//init optimum msg
	SetOptimumDataToString();

	//connector = new MYSQL();
	//sqlResult = new MYSQL_RES();

}
//Manager tcp network socket setting
void Manager::ConnectSetting(){
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
void* Manager::Connect(void *data){
	pthread_t thread_id = pthread_self();
	int fd;
	struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name,"wlan0",IFNAMSIZ-1);
	ioctl(fd,SIOCGIFADDR,&ifr);
	close(fd);
	printf("ipppp:%s\n",inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
	strcpy(myName,inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
	fd = socket(AF_INET,SOCK_DGRAM,0);
	while (1){
		
		printf("Server : waiting connection request. %d\n", thread_id);
		
		len = sizeof(clientAddr);
		clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &len);
		
		if (clientFd < 0)
		{
			printf("Server: accept failed.\n");
			exit(1);
		}
		else{
			pthread_mutex_lock(&mutexLock);
			printf("Server : client connected.\n");
			memset(buf, 0, strlen(buf));
			lenOut = read(clientFd, buf, sizeof(buf));
			strcpy(clientName, inet_ntoa(clientAddr.sin_addr));
			ManageMsg(buf);
			ShowMsg();
			close(clientFd);
			pthread_mutex_unlock(&mutexLock);
		}
	}
}
// call back function for thread
void* Manager::CallBackConnect(void* server_ptr){
	Manager* s = (Manager*)server_ptr;
	s->Connect(0);
}

void Manager::SetOptMsg(char msg[], int n){

	switch (n){
	case TEMP://T		
		strcpy(optimumTempMsg, msg);
		break;
	case HUMD://H		
		strcpy(optimumHumdMsg, msg);
		break;
	case LUX://L
		strcpy(optimumLuxMsg, msg);
		break;
	case MOTION://M
		strcpy(optimumMotionMsg, msg);
		break;
	default:
		perror("check sensor number");
		exit(1);

	}
}
//get, ConnectSetting socket field
struct sockaddr_in Manager::GetServerAddr(){
	return serverAddr;
}
struct sockaddr_in Manager::GetClientAddr(){
	return clientAddr;
}
int Manager::GetServerFd(){
	return serverFd;
}
int Manager::GetClientFd(){
	return clientFd;
}
void Manager::SetClientLen(sockaddr_in client){
	len = sizeof(client);
}
socklen_t Manager::GetClientLen(){
	return len;
}

//get,ConnectSetting optimum data 
int Manager::GetWeather(){
	return weather;
}
float Manager::GetOptimumTemp(){
	return optimumTemp;
}
float Manager::GetOptimumHumd(){
	return optimumHumd;
}
float Manager::GetOptimumLux(){
	return optimumLux;
}
float Manager::GetOptimumMotion(){
	return optimumMotion;
}

void Manager::SetWeather(int month){
	switch (month){
	case 3:
	case 4:
	case 5: weather = SPRNG_AUTUM;
		break;
	case 6:
	case 7:
	case 8: weather = SUMMER;
		break;
	case 9:
	case 10:
	case 11: weather = SPRNG_AUTUM;
		break;
	case 12:
	case 1:
	case 2: weather = WINTER;
		break;

	}
}
void Manager::SetOptimumTemp(float temp){
	optimumTemp = temp;
}
void Manager::SetOptimumHumd(float humd){
	optimumHumd = humd;
}
void Manager::SetOptimumLux(float lux){
	optimumLux = lux;
}
void Manager::SetOptimumMotion(float motion){
	optimumMotion = motion;
}

//print msg
void Manager::ShowMsg(){
	sensorManager->display();
	printf("\n");
}
//ConnectSetting optimum data with weather
//spring,autumn : 19~23'C, 50%% summer : 24~27 60%% winter: 18~21, 40%% ( especially Rhinitis : 50%%)
void Manager::SetOptimumDataWithWeather(int month){
	SetWeather(month);
	optimumLux = 55;
	optimumMotion = 0;
	switch (weather){
	case SPRNG_AUTUM:
		optimumTemp = 21;
		optimumHumd = 50;
		break;
	case SUMMER:
		optimumTemp = 25;
		optimumHumd = 60;
		break;
	case WINTER:
		optimumTemp = 20;
		optimumHumd = 40;
		break;
	default:
		perror("weather error()");
		break;
	}
}
//ConnectSetting optimum data msg
void Manager::SetOptimumDataToString(){
	char bufsT[BUFSIZ] = "";
	char datT[BUFSIZ + 1] = "UT";
	sprintf(bufsT, "%.2f", optimumTemp);
	strcat(datT, bufsT);
	SetOptMsg(datT, TEMP);

	char bufsH[BUFSIZ] = "";
	char datH[BUFSIZ + 1] = "UH";
	sprintf(bufsH, "%.2f", optimumHumd);
	strcat(datH, bufsH);
	SetOptMsg(datH, HUMD);

	char bufsL[BUFSIZ] = "";
	char datL[BUFSIZ + 1] = "UL";
	sprintf(bufsL, "%.2f", optimumLux);
	strcat(datL, bufsL);
	SetOptMsg(datL, LUX);

	char bufsM[BUFSIZ] = "";
	char datM[BUFSIZ + 1] = "UM";
	sprintf(bufsM, "%.2f", optimumMotion);
	strcat(datM, bufsM);
	SetOptMsg(datM, MOTION);
}

void Manager::SetOptimumStringToData(){
	char bufT[BUFSIZ];
	char bufH[BUFSIZ];
	char bufL[BUFSIZ];
	char bufM[BUFSIZ];
	int i;
	for (i = 2; i < strlen(optimumTempMsg); i++){
		bufT[i - 2] = optimumTempMsg[i];
	}
	for (i = 2; i < strlen(optimumHumdMsg); i++){
		bufH[i - 2] = optimumHumdMsg[i];
	}
	for (i = 2; i < strlen(optimumTempMsg); i++){
		bufL[i - 2] = optimumLuxMsg[i];
	}
	for (i = 2; i < strlen(optimumMotionMsg); i++){
		bufM[i - 2] = optimumMotionMsg[i];
	}
	sscanf(bufT, "%f", &optimumTemp);
	sscanf(bufH, "%f", &optimumHumd);
	sscanf(bufL, "%f", &optimumLux);
	sscanf(bufM, "%f", &optimumMotion);
}
void  Manager::ManageMsg(char buf[]){
	switch (buf[0]){
	case 'S':
		ReceiveSensorMsg(buf);
		break;
	case 'R':
		SendSensorMsg(buf);
		break;
	case 'U':
		ReceiveOptimumMsg(buf);printf("herrrerrererer\n");
		break;
	case 'O':
		SendOptimumMsg(buf);
		break;
	default:
		printf("read error");
		break;

	}
}
void  Manager::ReceiveSensorMsg(char buf[]){
	SmartThing* ps;
	switch (buf[1]){
	case 'T':
		if ((ps = sensorManager->isInList(TEMP, clientName)) != NULL) { ps->setMsg(buf); }
		else {

			SmartThing* sm ;
			sm = new SmartThing(clientName, TEMP, buf, optimumTempMsg);
			sensorManager->addLast(sm);
			sensorManager->AddTempLen();
		}
		break;
	case 'H':
		if ((ps = sensorManager->isInList(HUMD, clientName)) != NULL) { ps->setMsg(buf); }
		else {
			SmartThing* sm; 
			sm = new SmartThing(clientName, HUMD, buf, optimumHumdMsg);
			ps = sm;
			sensorManager->addLast(ps);
			sensorManager->AddHumdLen();
		}
		
		break;
	case 'L':
		if ((ps = sensorManager->isInList(LUX, clientName)) != NULL) { ps->setMsg(buf); }
		else {
			SmartThing* sm ;
			sm = new SmartThing(clientName, LUX, buf,optimumLuxMsg);
			ps = sm;
			sensorManager->addLast(ps);
			sensorManager->AddLuxLen();
		}
				break;
	case 'M':
		if ((ps = sensorManager->isInList(MOTION, clientName)) != NULL) { ps->setMsg(buf); }
		else {
			SmartThing* sm;
			char empty[BUFSIZ] = "";
			sm = new SmartThing(clientName, MOTION, buf, optimumMotionMsg);
			ps = sm;
			sensorManager->addLast(ps);
			sensorManager->AddMotionLen();

		}
		if (buf[2] == '1') {
			//pthread_mutex_lock(&mutexLock);
			CommuniSetting();
			sprintf(query, "insert into motionSensor values(%d,'%s',now(),1)", myCode, ps->ip);
			WriteQuery();
			//pthread_mutex_unlock(&mutexLock);
		}
		break;
	}
	sensorManager->SensorErrorCheck();
}
void  Manager::SendSensorMsg(char buf[]){
	SmartThing* ps;
	memset(bufIpz, 0, strlen(bufIpz));
	MsgCutForGetSendIpAddr(buf);
	switch (buf[1]){
	case 'T':
		if ((ps = sensorManager->isInList(TEMP, bufIpz)) != NULL) { write(clientFd,ps->msg, sizeof(ps->msg)); }
		else {
			write(clientFd, emptyTempMsg, sizeof(emptyTempMsg));
		}
		break;
	case 'H':
		if ((ps = sensorManager->isInList(HUMD, bufIpz)) != NULL) { write(clientFd, ps->msg, sizeof(ps->msg)); }
		else {
			write(clientFd, emptyHumdMsg, sizeof(emptyHumdMsg));
		}
		break;
	case 'L':
		if ((ps = sensorManager->isInList(LUX, bufIpz)) != NULL) { write(clientFd, ps->msg, sizeof(ps->msg)); }
		else {
			write(clientFd, emptyLuxMsg, sizeof(emptyLuxMsg));
		}
		break;
	case 'M':
		if ((ps = sensorManager->isInList(MOTION, bufIpz)) != NULL) { write(clientFd, ps->msg, sizeof(ps->msg)); }
		else {
			write(clientFd, emptyMotionMsg, sizeof(emptyMotionMsg));
		}
		break;
	}

}
char* Manager::MsgCutForGetSendIpAddr(char buf[]) {
	for (int i = 2; i < strlen(buf); i++) {
		bufIpz[i - 2] = buf[i];
	}
	return bufIpz;
}
void  Manager::ReceiveOptimumMsg(char buf[]){
	SmartThing* ps;
	char iplench[BUFSIZ];
        int j;
        int iplen;
	for(j=0; j<=1; j++)
	{iplench[j] = buf[j+2];}
	sscanf(iplench,"%d",&iplen);
	char optIp[BUFSIZ];
	char optData[BUFSIZ];
	int i;
	for (i = 0; i < iplen; i++) {
		optIp[i] = buf[i + 4];
	}
        optData[0] = buf[0];
	optData[1] = buf[1];
	for (i=0; i < strlen(buf); i++) {
		optData[i+2] = buf[i+iplen+4];
	}
	printf("OPTTTIP%s\n",optIp);
	printf("optData:%s\n",optData);
	switch (buf[1]){
	case 'T':
		if ((ps = sensorManager->isInList(TEMP, optIp)) != NULL) 
		{
			ps->setOptMsg(optData);
			printf("set opt data%s",optData);
		}
		break;
	case 'H':
		if ((ps = sensorManager->isInList(HUMD, optIp)) != NULL)
		{
			ps->setOptMsg(optData);
		}
		break;
	case 'L':
		if ((ps = sensorManager->isInList(LUX, optIp)) != NULL)
		{
			ps->setOptMsg(optData);
		}
		break;
	case 'M':
		if ((ps = sensorManager->isInList(MOTION, optIp)) != NULL)
		{
			ps->setOptMsg(optData);
		}
		break;
	}
}
void  Manager::SendOptimumMsg(char buf[]){
	SmartThing* ps;
	switch (buf[1]){
	case 'T':
		if ((ps = sensorManager->isInList(TEMP, clientName)) != NULL) { write(clientFd, ps->opt, sizeof(ps->opt)); }
		else {
			write(clientFd, optimumTempMsg, sizeof(optimumTempMsg));
		}
		break;
	case 'H':
		if ((ps = sensorManager->isInList(HUMD, clientName)) != NULL) { write(clientFd, ps->opt, sizeof(ps->opt)); }
		else {
			write(clientFd, optimumHumdMsg, sizeof(optimumHumdMsg));
		}break;
	case 'L':
		if ((ps = sensorManager->isInList(LUX, clientName)) != NULL) { write(clientFd, ps->opt, sizeof(ps->opt)); }
		else {
			write(clientFd, optimumLuxMsg, sizeof(optimumLuxMsg));
		}break;
	case 'M':
		if ((ps = sensorManager->isInList(MOTION, clientName)) != NULL) { printf("send opt%s", ps->opt); write(clientFd, ps->opt, sizeof(ps->opt)); }
		else {
			printf("send opt%s", optimumMotionMsg);
			write(clientFd, optimumMotionMsg, sizeof(optimumMotionMsg));
		}
		break;
	}
}
//Manager destructor 
Manager::~Manager(){
	close(serverFd);
}

//------------ Communication with Server ------------------------------------------------------------------------------------
// call back function for thread
void* Manager::CallBackCommuniConnect(void* server_ptr) {
	Manager* s = (Manager*)server_ptr;
	s->SendCommuniMsg();
}
void Manager::CommuniSetting(){
	
	mysql_init(&connector);
	if (!mysql_real_connect(&connector, DBHOST, DBUSER, DBPASS, DBNAME, 3306,NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(&connector));
		return;
	}
	printf("data base open..\n");

}
void Manager::SendCommuniMsg(){
	time_t timer;
	struct tm *t;
	bool once = false;
	ManagerSetQuery();
	
	while (1){	
			timer = time(NULL);
			t = localtime(&timer);
			int hour, min, sec;
			hour = t->tm_hour;
			min = t->tm_min;
			sec = t->tm_sec;
			if(min==0){
				if(!once){
					CommuniSetting();
					once = true;
					printf("hour: %d min:%d sec:%d",hour,min,sec);
					pthread_mutex_lock(&mutexLock);
					printf("DBWAIT\n");
					ManagerSetQuery();
					WriteAllData(hour);
					pthread_mutex_unlock(&mutexLock);
					mysql_close(&connector);
				}
			}
			else{
				once= false;
			}
		
		
	}
}
static int first =0;
void Manager::ManagerSetQuery() {
	
	char myIp[BUFSIZ] = "192.168.0.11";
	//strcpy(myIp,myName);
	printf("myName:%s myIp%s\n",myName,myIp);
	
	if (first==0) {
		first++;
		sprintf(query, "insert into Manager values(NULL,'%s', %d,%d,%d,%d)", myIp, sensorManager->GetTempLen(), sensorManager->GetHumdLen(),
			sensorManager->GetLuxLen(), sensorManager->GetMotionLen());
		WriteQuery();
		sprintf(query, "select managerCode from Manager where managerIp = '%s'",myIp);
		WriteQuery();
		sqlResult = mysql_store_result(&connector);
		while((sqlRow = mysql_fetch_row(sqlResult)) != NULL){ sscanf(sqlRow[0],"%d",&myCode); printf("myCode!! %d",myCode);} 
		mysql_free_result(sqlResult);
	}
	else {
		sprintf(query, "update Manager set temp = %d , humd= %d, lux = %d, motion = %d where managerIp = '%s'", sensorManager->GetTempLen(), sensorManager->GetHumdLen(),
		sensorManager->GetLuxLen(), sensorManager->GetMotionLen(),myIp);
		WriteQuery();
	}
	
}
void Manager::WriteAllData(int hour) {
	int i;
	float data;
	float optValue;
	int time = hour;
	char type[BUFSIZ];
	SmartThing* node;
	int one=1;
	for(int i=0; i< sensorManager->getLength(); i++){	 
		node = sensorManager->getSmartThingAt(i);
		data = MsgTransformToQuery(node->msg);
		char buf[BUFSIZ];
		strcpy(buf,node->opt);
		if(buf[2]=='F'){one=0;}
		if (node->sensorType == TEMP) { strcpy(type, "tempSensor"); optValue = node->getOptVal(); }
		if (node->sensorType == HUMD) { strcpy(type, "humdSensor"); optValue = node->getOptVal(); }
		if (node->sensorType == LUX) { strcpy(type, "luxSensor"); optValue = node->getOptVal();}
		sprintf(query, "insert into %s values(%d,'%s',%f,%f,%d,now(),%d)", type,myCode,node->ip, data, optValue,hour,one);
		WriteQuery();
	}
}
float Manager::MsgTransformToQuery(char msg[]) {
	float result;
	char buff[BUFSIZ];
	for (int i = 2; i < strlen(msg); i++) {
		buff[i - 2] = msg[i];
	}
	sscanf(buff, "%f.2", &result);
	return result;
}
void Manager::WriteQuery() {
	int res;
	if (res = mysql_query(&connector, query)) {
		fprintf(stderr, "%s\n", mysql_error(&connector));
		printf("write db error\n");
	}
	else { printf("write success\n"); }
	memset(query, 0, strlen(query));
}