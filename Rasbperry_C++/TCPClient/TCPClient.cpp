/*----------------------------------------------------------------------------------------------
파일명 : TCP_client.cpp
기 능  : 클라이언트 통신 클래스 함수 구현

----------------------------------------------------------------------------------------------*/
#include "TCPClient.h"
#define TCP_PORT 5100
#define TEMP 1
#define HUMD 2
#define LUX 3
#define MOTION 4
/***********Client class function area***********/
//Client constructor
Client::Client(){
	char init[BUFSIZ] = "";
	char ips[] = "ipNumber"
	MsgSetting(init, 1);
	MsgSetting(init, 2);
	MsgSetting(init, 3);
	SetIp(ips);
}
//communication ready 
void Client::ConnectionSetting(char* argv){

	//construct socket use ip4 internet protocol
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket()");
		exit(1);
	}
	//register Client address and service in OS 
	on = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;

	//hton  host to network 
	serverAddr.sin_addr.s_addr = inet_addr(argv);
	serverAddr.sin_port = htons(TCP_PORT);
	serverAddrLen = sizeof(serverAddr);

}
//set Temp,Humd,Lux data 
void Client::MsgSetting(char Data[], int n){

	switch (n){
	case TEMP://T
		strcpy(tempMsg, Data);
		break;
	case HUMD://H
		strcpy(humdMsg, Data);
		break;
	case LUX://L
		strcpy(luxMsg, Data);
		break;
	case MOTION://L
		strcpy(motionMsg, Data);
		break;
	default:
		perror("check sensor number");
		exit(1);
	}

}
//print Data
void Client::PrintMsg()const{

	printf("received Tdata:%s\n", tempMsg);
	printf("received Hdata:%s\n", humdMsg);
	printf("received Ldata:%s\n", luxMsg);
	printf("received Mdata:%s\n", motionMsg);
	printf("\n");

}
//destroy Client
Client::~Client(){

}
//get functions
socklen_t Client::GetServerAddrLen(){
	return serverAddrLen;
}
struct sockaddr* Client::GetServerAddr(){
	return (struct sockaddr *)&serverAddr;
}
int Client::GetSockAddr(){
	return sock;
}
char* Client::GetCommunicationOptimumMsgToSend(){
	return communicationOptimumMsgToSend;
}
char* Client::GetCommunicationSensorMsgToSend(){
	return  communicationSensorMsgToSend;
}
char* Client::GetTempMsg(){
	return tempMsg;
}
char* Client::GetHumdMsg(){
	return humdMsg;
}
char* Client::GetLuxMsg(){
	return luxMsg;
}
char* Client::GetMotionMsg(){
	return motionMsg;
}
//set functions
void Client::SetSockAddr(int addr){
	sock = addr;
}
void Client::SetCommunicationOptimumMsgToSend(char msg[]){
	strcpy(communicationOptimumMsgToSend, msg);
}
void Client::SetCommunicationSensorMsgToSend(char msg[]){
	strcpy(communicationSensorMsgToSend, msg);
}
void Client::SetTempMsg(char data[]){
	strcpy(tempMsg, data);
}
void Client::SetHumdMsg(char data[]){
	strcpy(humdMsg, data);
}
void Client::SetLuxMsg(char data[]){
	strcpy(luxMsg, data);
}
void Client::SetMotionMsg(char data[]){
	strcpy(motionMsg, data);
}

char* Client::GetIp(){
	return ip;
}
void Client::SetIp(char ips[]){
	strcpy(ip, ips);
}
