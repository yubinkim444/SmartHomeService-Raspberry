/*----------------------------------------------------------------------------------------------
파일명 : THL_sensor.cpp
기 능  : gpio 센서 클래스

----------------------------------------------------------------------------------------------*/
#include "Sensor.h"

#define CS_MCP3208 6//BCM_GPIO 25  
#define SPI_CHANNEL 0      
#define SPI_SPEED   1000000  // 1MHz
//constructor

Sensor::Sensor(int pn, unsigned char channel,int pr){
	sensorType = TYPE_INIT;
	mcpData = DATA_INIT;
	volt = VOLT_INIT;
	realData = DATA_INIT;
	timerOn = false;
	electronicOnTime = 1000 * 60 * 3;
	pin = pn;
	pir = pr;
	spiChannel = channel;
	SensorSetting();
	RelaySetting();
}

Sensor::Sensor(int pn, unsigned char channel){
	sensorType = TYPE_INIT;
	mcpData = DATA_INIT;
	volt = VOLT_INIT;
	realData = DATA_INIT;
	timerOn = false;
	electronicOnTime = 1000 * 60 * 3;
	pin = pn;
	spiChannel = channel;
	SensorSetting();
	RelaySetting();
}

//Settings
int Sensor::SetPi(){
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
		return -1;
	}
	return 0;
	pinMode(pir, INPUT);//PIR setting
}
int Sensor::SetSpi(){
	if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)
	{
		fprintf(stdout, "wiringPiSPISetup Failed: %s\n", strerror(errno));
		return -1;
	}
	
	return 0;
}
void Sensor::SetSensorType(int type){
	sensorType = type;
}
//Read data
int Sensor::Detection()
{
	unsigned char adcChannel = spiChannel;
	unsigned char buff[3];
	int adcValue = 0;
	buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
	buff[1] = ((adcChannel & 0x07) << 6);
	buff[2] = 0x00;

	digitalWrite(CS_MCP3208, 0);  // Low : CS Active

	wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);

	buff[1] = 0x0F & buff[1];
	adcValue = (buff[1] << 8) | buff[2];

	digitalWrite(CS_MCP3208, 1);  // High : CS Inactive
	mcpData = adcValue;
	volt = McpDataToVolt();
	printf("adcVlaue:%d\n",adcValue);
	return adcValue;
}
int Sensor::MotionDetection(){
	mcpData = digitalRead(pir);
	volt = mcpData;
	return mcpData;
}
int Sensor::SensorSetting(){
	SetPi();
	SetSpi();
	return 0;
}
int Sensor::GetMcpData(){
	return mcpData;
}
float Sensor::GetVolt(){
	return volt;
}
void Sensor::SetVolt(float vol){
	volt = vol;
}
float Sensor::GetRealData(){
	return realData;
}
void Sensor::SetRealData(float realValue){
	realData = realValue;
}
bool Sensor::GetTimerOn() {
	return timerOn;
}
void Sensor::SetTimerOn(bool on) {
	timerOn = on;
}
int Sensor::GetElectronicOnTime() {
	return electronicOnTime;
}
clock_t Sensor::GetStartTime() {
	return startTime;
}
//Convert data
float Sensor::McpDataToVolt(){
	volt = (5.0 / 4096)*(float)mcpData;
	return volt;
}
int Sensor::ReadMcpData(){
	Detection();
	return mcpData;
}
int Sensor::ReadMotionData(){
	MotionDetection();
	return mcpData;
}
void Sensor::ShowData(){
	printf("mcpData: %d\n volt: %f\n realData :%f", mcpData, volt, realData);
}
//actuate electronic
void Sensor::RelaySetting(){
		pinMode(pin, OUTPUT);
}
void Sensor::ElectronicOn(){
	timerOn = true;
	startTime = clock();
	digitalWrite(pin, HIGH);
}
void Sensor::ElectronicOff(){
	if (!timerOn) {
		digitalWrite(pin, LOW);
	}
}





