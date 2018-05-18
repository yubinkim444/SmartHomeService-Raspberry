/*----------------------------------------------------------------------------------------------
파일명 : SmartSensor.cpp
기 능  : 스마트 센서 클래스 함수 구현 

----------------------------------------------------------------------------------------------*/
#include "SmartSensor.h"

#define TEMP 1
#define HUMD 2
#define LUX 3
#define MOTION 4
/****************SmartSensorclass function area************************/
//SmartSensor constructor
SmartSensor::SmartSensor(int pn, unsigned char channel) : Sensor(pn, channel){
	
}
SmartSensor::SmartSensor(int pn, unsigned char channel, int pir) : Sensor(pn, channel,pir){
	
}
SmartSensor::~SmartSensor(){
	//no action
}
//set SendMyData for ReceiveOtherData msg(RT,RH,RL)
void SmartSensor::CommunicationMsgSetting(int type){
	char bufs[BUFSIZ];	
	switch (type){
	case TEMP:
		strcpy(bufs, "RT");
		SetCommunicationSensorMsgToSend(bufs);
		
		break;
	case HUMD:
		strcpy(bufs, "RH");
		SetCommunicationSensorMsgToSend(bufs);
		break;
	case LUX:
		strcpy(bufs, "RL");
		SetCommunicationSensorMsgToSend(bufs);		
		break;
	case MOTION:
		strcpy(bufs, "RM");
		SetCommunicationSensorMsgToSend(bufs);
		break;
	}

}
void SmartSensor::Sleep(){
	for (int i = 0; i < 55000000; i++);
}

void SmartSensor::SetOptimumMsg(char optimum[]){
	strcpy(optimumMsg, optimum);
}
char* SmartSensor::GetOptimumMsg(){
	return optimumMsg;
}
void SmartSensor::OptimumMsgToData(){
	sscanf(optimumMsg, "%f", &optimumData);
}
float SmartSensor::GetOptimumData(){
	return optimumData;
}
void SmartSensor::SetOptimumData(float optData){
	optimumData = optData;
}
float SmartSensor::GetTolerance(){
	return tolerance;
}
void SmartSensor::SetTolerance(float tol){
	tolerance = tol;
}
void SmartSensor::ReceiveOptimumMsg(){
	
	char buf[BUFSIZ];
	ConnectionSetting(GetIp());
	if (connect(GetSockAddr(), GetServerAddr(), GetServerAddrLen()) < 0){
		perror("ReceiveOtherData connect()");
		exit(1);
	}
	bzero(buf, sizeof(buf));

	if (write(GetSockAddr(), GetCommunicationOptimumMsgToSend(), sizeof(GetCommunicationOptimumMsgToSend())) <= 0){
		perror("ReceiveOtherData optimum write()");
	}

	if ((read(GetSockAddr(), buf, sizeof(buf))) <= 0){
		perror("ReceiveOtherData read()");
	}
	else {
		if (buf[2] == 'F') {
			strcpy(optimumMsg,"OFF");
			//optimumData = -777;
		}
		else {
			char handle_buf[BUFSIZ];
			int i;
			for (i = 2; i < sizeof(buf); i++) {
				handle_buf[i - 2] = buf[i];
			}
			SetOptimumMsg(handle_buf);
			OptimumMsgToData();
		}
	}
	close(GetSockAddr());

}
void SmartSensor::Start(void* data){
	bool sendOrReceive = true;
        ReceiveOptimumMsg();
	while (1){
		if (sendOrReceive){
			SendMyData();
			Decision();
			sendOrReceive = false;
		}
		else{
			ReceiveOptimumMsg();
			sendOrReceive = true;
		}
		sleep(1);
		PrintMsg();
	}
}
void* SmartSensor::CallBackStart(void* ptr){
	SmartSensor* s = (SmartSensor*)ptr;
	s->Start(0);
}
/************Temp,Humd,Lux SmartSensor class  function area*************/
//TempSmartSensor constructor (set sens type)
TempSmartSensor::TempSmartSensor(int pin, unsigned char channel):SmartSensor(pin,channel){
	char buff[BUFSIZ];
	SetTolerance(4); //set tolerance +- 2'C
	SetSensorType(TEMP);
	//init optimum ReceiveOtherData msg
	strcpy(buff, "OT");
	SetCommunicationOptimumMsgToSend(buff);
}
float TempSmartSensor::VoltToRealData(){
//not already yet
	float temp_real_data = (float)GetVolt()*100.0;
	
	SetRealData(temp_real_data);
	return temp_real_data;
}
float TempSmartSensor::MyRealData(){
	ReadMcpData();
	VoltToRealData();
	return GetRealData();
}
// set TempSmartSensor SendMyData msg(for SendMyData TempMsg)
 void TempSmartSensor::SendMsgSetting(float data){
	char bufs[BUFSIZ] = "";
	char dat[BUFSIZ + 1] = "ST";
	sprintf(bufs, "%.2f", data);
	strcat(dat, bufs);
	MsgSetting(dat, TEMP);
}

//SendMyData TempMsg
 void TempSmartSensor::SendMyData(){//SendMyData STdata msg
	
	char buf[BUFSIZ];
	ConnectionSetting(GetIp());
	if (connect(GetSockAddr(), GetServerAddr(), GetServerAddrLen()) < 0){
		perror("SendMyData connect()");
		exit(1);
	}
	
	SendMsgSetting(MyRealData());
	if (write(GetSockAddr(), GetTempMsg(),BUFSIZ) <= 0){
		perror("SendMyData write()");

	}
	
	close(GetSockAddr());
	
}
 
void TempSmartSensor::Decision(){
	
	char* msg =GetOptimumMsg();
	 
	if (!strcmp(msg, "OFF")) { 
		printf("OFF MODE..\n");
		SetTimerOn(false);
		ElectronicOff(); }
	else {
		CalculateDecision();
	}
}
void  TempSmartSensor::CalculateDecision() {
	bool plusTemp = false;
	pthread_mutex_lock(&mutexLock);
	float tempValue = GetRealData();
	pthread_mutex_unlock(&mutexLock);
	float optValue = GetOptimumData();
	if (optValue >= 100) { plusTemp = true; optValue -= 100; }
	float toleranceValue = GetTolerance();
	float devTolerance = (toleranceValue / 2) + 0.5;
	printf("opt value: %.2f", optValue);
	if (!GetTimerOn()) {
		if (tempValue<(optValue + devTolerance) && tempValue >(optValue - devTolerance)) {
			ElectronicOff();
			printf("Temperature is good \n");
		}
		else if (tempValue < (optValue - devTolerance)) {
			printf("Temperature is low.\n");
			if (plusTemp) {
				ElectronicOn();
			}
			else {
				ElectronicOff();
			}
		}
		else {
			printf("Temperature is high.\n");
			if (plusTemp) {
				ElectronicOff();
			}
			else {
				ElectronicOn();
			}
		}
	}
	else {
		if (clock() - GetStartTime() > GetElectronicOnTime()) {
			SetTimerOn(false);
		}
	}
}
//HumdSmartSensor constructor
HumdSmartSensor::HumdSmartSensor(int pin, unsigned char channel):SmartSensor(pin,channel) {
	char buff[BUFSIZ];
	SetSensorType(HUMD);
	//set tolerance +-5%%
	SetTolerance(16);
	//init optimum ReceiveOtherData msg
	strcpy(buff, "OH");
	SetCommunicationOptimumMsgToSend(buff);
}

float HumdSmartSensor::VoltToRealData(){
	//not already yet
	float humd_real_data = ((float)GetVolt()) * (1.0/33.0)*1000.0;
	//range 30%%~90%%(60..)
	SetRealData(humd_real_data);
	return humd_real_data;
}
float HumdSmartSensor::MyRealData(){
	ReadMcpData();
	VoltToRealData();
	return GetRealData();
}
// set HumdSmartSensor SendMyData msg(for SendMyData HumdMsg)
 void HumdSmartSensor::SendMsgSetting(float data){
	char bufs[BUFSIZ] = "";
	char dat[BUFSIZ + 1] = "SH";
	sprintf(bufs, "%.2f", data);
	strcat(dat, bufs);
	MsgSetting(dat, HUMD);
}
//SendMyData HumdMsg
void HumdSmartSensor::SendMyData(){

	char buf[BUFSIZ];
	ConnectionSetting(GetIp());
	if (connect(GetSockAddr(), GetServerAddr(), GetServerAddrLen()) < 0){
		perror("SendMyData connect()");
		exit(1);
	}
	SendMsgSetting(MyRealData());
	if (write(GetSockAddr(), GetHumdMsg(), BUFSIZ) <= 0){
		perror("SendMyData write()");
	}	
	close(GetSockAddr());
}

 void HumdSmartSensor::Decision(){
	 char* msg =GetOptimumMsg();
	 if (!strcmp(msg, "OFF")) {
		 printf("OFF MODE..\n");SetTimerOn(false);
		 ElectronicOff();
	 }
	 else {
		 CalculateDecision();
	 }
 }
 void HumdSmartSensor::CalculateDecision(){
	 bool plusHumd = true;
	 float humdValue = GetRealData();
	 float optValue = GetOptimumData();
	 if (optValue >= 100) { plusHumd = false; optValue -= 100; }
	 float toleranceValue = GetTolerance();
	 float devTolerance = (toleranceValue / 2) + 0.5;
	 printf("opt value: %.2f", optValue);
	 if (!GetTimerOn()) {
		 if (humdValue<(optValue + devTolerance) && humdValue >(optValue - devTolerance)) {
			 printf("Humdity is good \n");
			 ElectronicOff();
		 }
		 else if (humdValue < (optValue - devTolerance)) {
			 printf("Humidity is low.\n");
			 if (plusHumd) {
				 ElectronicOn();
			 }
			 else { ElectronicOff(); }
		 }
		 else {
			 printf("Humidity is high.\n");
			 if (plusHumd) {
				 ElectronicOff();
			 }
			 else { ElectronicOn(); }
		 }
	 }
	 else {
		 if (clock() - GetStartTime() > GetElectronicOnTime()) {
			 SetTimerOn(false);
		 }
	 }
 }
//LuxSmartSensor constructor
LuxSmartSensor::LuxSmartSensor(int pin, unsigned char channel):SmartSensor(pin,channel){
	char buff[BUFSIZ];
	SetSensorType(LUX);
	SetTolerance(20); // set tolerance +-10
	//init optimum ReceiveOtherData msg
	strcpy(buff, "OL");
	SetCommunicationOptimumMsgToSend(buff);
}
float LuxSmartSensor::VoltToRealData(){
	//not already yet
	float lux_real_data = (((float)GetMcpData())/4096)*100.0;
	SetRealData(lux_real_data);
	return lux_real_data;
}
float LuxSmartSensor::MyRealData(){
	ReadMcpData();
	VoltToRealData();
	return GetRealData();
}
// set LuxSmartSensor SendMyData msg(for SendMyData LuxMsg)
 void LuxSmartSensor::SendMsgSetting(float data){
	char bufs[BUFSIZ];
	char dat[BUFSIZ + 1] = "SL";
	sprintf(bufs, "%.2f", data);
	strcat(dat, bufs);
	MsgSetting(dat, LUX);
}
//SendMyData LuxMsg
 void LuxSmartSensor::SendMyData(){
	 
	char buf[BUFSIZ];
	ConnectionSetting(GetIp());
	if (connect(GetSockAddr(), GetServerAddr(), GetServerAddrLen()) < 0){
		perror("SendMyData connect()");
		exit(1);
	}
	SendMsgSetting(MyRealData());
	if (write(GetSockAddr(), GetLuxMsg(), BUFSIZ) <= 0){
		perror("SendMyData write()");

	}
	
	close(GetSockAddr());
}

 void LuxSmartSensor::Decision(){
	 char* msg =GetOptimumMsg();
	 if (!strcmp(msg, "OFF")) {
		 printf("OFF MODE..\n");SetTimerOn(false);
		 ElectronicOff();
	 }
	 else {
		 CalculateDecision();
	 }
 }
 void LuxSmartSensor::CalculateDecision() {
	 float lux_value = GetRealData();
	 float optValue = GetOptimumData();
	 float toleranceValue = GetTolerance();
	 float devTolerance = (toleranceValue / 2) + 0.5;
	 printf("opt value: %.2f", optValue);
	 if (!GetTimerOn()) {
		 if (lux_value<(optValue + devTolerance) && lux_value >(optValue - devTolerance)) {
			 printf("Lux is good \n");
			 ElectronicOff();
		 }
		 else if (lux_value < (optValue - devTolerance)) {
			 printf("Lux is low.\n");
			 ElectronicOn();
		 }
		 else {
			 printf("Lux is high.\n");
			 ElectronicOff();
		 }
	 }
	 else {
		 if (clock() - GetStartTime() > GetElectronicOnTime()) {
			 SetTimerOn(false);
		 }
	 }
 }
 MotionSmartSensor::MotionSmartSensor(int pin, unsigned char channel,int pir):SmartSensor(pin,channel,pir){
	 char buff[BUFSIZ];
	 SetTolerance(0); //set tolerance +- 2'C
	 SetSensorType(MOTION);
	 //init optimum ReceiveOtherData msg
	 strcpy(buff, "OM");
	 SetCommunicationOptimumMsgToSend(buff);
 }
 float MotionSmartSensor::VoltToRealData(){
	 //not already yet
	 float motionRealData = (float)GetMcpData();

	 SetRealData(motionRealData);
	 return motionRealData;
 }
 float MotionSmartSensor::MyRealData(){
	 ReadMotionData();
	 VoltToRealData();
	 return GetRealData();
 }
 // set TempSmartSensor SendMyData msg(for SendMyData TempMsg)
 void MotionSmartSensor::SendMsgSetting(float data){
	 char bufs[BUFSIZ] = "";
	 char dat[BUFSIZ + 1] = "SM";
	 sprintf(bufs, "%.2f", data);
	 strcat(dat, bufs);
	 MsgSetting(dat, MOTION);
 }

 //SendMyData TempMsg
 void MotionSmartSensor::SendMyData(){//SendMyData STdata msg

	 char buf[BUFSIZ];
	 ConnectionSetting(GetIp());
	 if (connect(GetSockAddr(), GetServerAddr(), GetServerAddrLen()) < 0){
		 perror("SendMyData connect()");
		 exit(1);
	 }

	 SendMsgSetting(MyRealData());
	 if (write(GetSockAddr(), GetMotionMsg(), BUFSIZ) <= 0){
		 perror("SendMyData write()");

	 }
	 close(GetSockAddr());
 }
 /*
 //ReceiveOtherData HumdMsg, LuxMsg
 void MotionSmartSensor::ReceiveOtherData(int type){//recieve other sens_data (SendMyData RT,RH,RL msg)

	 char buf[BUFSIZ];
	 CommunicationMsgSetting(type);
	 ConnectionSetting(GetIp());
	 if (connect(GetSockAddr(), GetServerAddr(), GetServerAddrLen()) < 0){
		 perror("ReceiveOtherData connect()");
		 exit(1);
	 }
	 //read data
	 bzero(buf, sizeof(buf));

	 if (write(GetSockAddr(), GetCommunicationSensorMsgToSend(), sizeof(GetCommunicationSensorMsgToSend())) <= 0){
		 perror("ReceiveOtherData write()");
	 }

	 if ((read(GetSockAddr(), buf, sizeof(buf))) <= 0){
		 perror("ReceiveOtherData read()");

	 }
	 else{
		 switch (buf[1]){
		 case 'T':
			 MsgSetting(buf, TEMP);
			 break;
		 case 'H':
			 MsgSetting(buf, HUMD);
			 break;
		 case 'L':
			 MsgSetting(buf, LUX);
			 break;
		 default:
			 printf("no correct data\n");
			 break;

		 }
	 }
	 close(GetSockAddr());
 }
 */
 void MotionSmartSensor::Decision(){
	 char* msg =GetOptimumMsg();
	 if (!strcmp(msg, "OFF")) {
		 printf("OFF MODE..\n");SetTimerOn(false);
		 ElectronicOff();
	 }
	 else {
		 CalculateDecision();
	 }	 
 }
 void MotionSmartSensor::CalculateDecision() {
	 float motionValue = GetRealData();
	 if (motionValue == 1) {
		 printf("Detection\n");
		 ElectronicOn();
		 delay(500);
		 SetTimerOn(false);
		 ElectronicOff();
	 }
	 else {
		 printf("Not Detection\n");
		 ElectronicOff();
	 }
 }