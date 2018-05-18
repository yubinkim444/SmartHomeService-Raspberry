
#include "LinkedList.h"

SmartThing::SmartThing(char ip[], int sensorType, char msg[],char opt[]) {
	this->ip = new char[strlen(ip)+1];
	this->msg = new char[strlen(msg)+1];
	this->opt = new char[strlen(opt)+1];
	strcpy(this->msg, msg);
	strcpy(this->opt, opt);
	strcpy(this->ip, ip);
	this->sensorType = sensorType;
	this->timer = clock();
}
void SmartThing::setMsg(char buf[]) {
	strcpy(this->msg, buf);
	timer = clock();
}
SmartThing::~SmartThing(){
delete this;

}
void SmartThing::setOptMsg(char buf[]) {
	strcpy(this->opt, buf);
}
float SmartThing::getOptVal(){
	int i; float val=0;
	char buf[BUFSIZ];	
	for(i=2;i<sizeof(opt);i++){
		buf[i-2] = opt[i];
	}
	sscanf(buf,"%f",&val);
	return val;
}
// 리스트를 초기화한다.
SensorManager::SensorManager()
{
	length = 0;
	tempLen = 0;
	humdLen = 0;
	luxLen = 0;
	motionLen = 0;
	head = NULL;
	phead = &head;
}
// phead: 리스트의 헤드 포인터의 포인터
// p : 선행 노드
// new_node : 삽입될 노드 
void SensorManager::insertSmartThing(SmartThing *p, SmartThing *new_node)
{
	if (*phead == NULL) {	// 공백리스트인 경우
		new_node->link = NULL;
		*phead = new_node;
	}
	else if (p == NULL) { // p가 NULL이면 첫번째 노드로 삽입
		new_node->link = *phead;
		*phead = new_node;
	}
	else {			 // p 다음에 삽입
		new_node->link = p->link;
		p->link = new_node;
	}
}
// phead : 헤드 포인터에 대한 포인터 
// p: 삭제될 노드의 선행 노드
// removed: 삭제될 노드 
void SensorManager::removeSmartThing(SmartThing *p, SmartThing *removed)
{
	if (p == NULL)
		*phead = (*phead)->link;
	else
		p->link = removed->link;
	free(removed);
}

// 리스트안에서 pos 위치의 노드를 반환한다.
SmartThing * SensorManager::getSmartThingAt(int pos)
{
	int i;
	SmartThing *tmp_node = head;
	if (pos < 0) return NULL;
	for (i = 0; i<pos; i++)
		tmp_node = tmp_node->link;
	return tmp_node;
}
// 리스트의 항목의 개수를 반환한다.
int SensorManager::getLength()
{
	return length;
}
//
void SensorManager::error(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}
// 주어진 위치에 데이터를 삽입한다.
void SensorManager::add(int position, SmartThing* newThing)
{
	SmartThing *p;
	if ((position >= 0) && (position <= length)) {
		p = getSmartThingAt(position - 1);
		insertSmartThing(p, newThing);
		length++;
	}
}
// 리스트의 끝에 데이터를 삽입한다.
void SensorManager::addLast(SmartThing* newThing)
{
	add(getLength(), newThing);
}
// 리스트의 끝에 데이터를 삽입한다.
void SensorManager::addFirst(SmartThing* newThing)
{
	add(0, newThing);
}
//
int SensorManager::isEmpty()
{
	if (head == NULL) return 1;
	else return 0;
}

// 주어진 위치의 데이터를 삭제한다.
void SensorManager::deleteSmartThing(int pos)
{
	if (!isEmpty() && (pos >= 0) && (pos < length)) {
		SmartThing *p = getSmartThingAt(pos - 1);
		removeSmartThing(p, (p != NULL) ? p->link : NULL);
		length--;
	}
}
//
SmartThing* SensorManager::getEntry(int pos)
{
	SmartThing *p;
	if (pos >= length) return NULL;
	p = getSmartThingAt(pos);
	return p;
}
//
void SensorManager::clear()
{
	int i;
	for (i = 0; i<length; i++)
		deleteSmartThing(i);
}
// 버퍼의 내용을 출력한다. 
void SensorManager::display()
{
	int i;
	SmartThing *node = head;
	printf("( ");
	for (i = 0; i<length; i++) {
		printf("ip : %s ,type: %d data: %s opt:%s\n", node->ip, node->sensorType, node->msg,node->opt);
		node = node->link;
	}
	printf(" )\n");
}
// 데이터 값이 s인 노드를 찾는다.
SmartThing* SensorManager::isInList(int type, char ips[])
{
	SmartThing *p;
	p = head; 	// 헤드 포인터에서부터 시작한다.
	while ((p != NULL)) {
		// 노드의 데이터가 item이면
		if (p->sensorType == type && strcmp(p->ip, ips) == 0)
			break;
		p = p->link;
	}
	if (p == NULL) return NULL;
	else return p;
}
void SensorManager::AddTempLen() {
	this->tempLen += 1;
}
void SensorManager::AddHumdLen() {
	this->humdLen += 1;
}
void SensorManager::AddLuxLen() {
	this->luxLen += 1;
}
void SensorManager::AddMotionLen() {
	this->motionLen += 1;
}

void SensorManager::SubTempLen() {
	this->tempLen -= 1;
}
void SensorManager::SubHumdLen() {
	this->humdLen -= 1;
}
void SensorManager::SubLuxLen() {
	this->luxLen -= 1;
}
void SensorManager::SubMotionLen() {
	this->motionLen -= 1;
}
void SensorManager::SensorErrorCheck() {
	SmartThing *p;
	p = head; 
	int i = 1;	
	while ((p != NULL)) {
		if ((clock() - (p->timer))/(double)CLOCKS_PER_SEC > 180){
			deleteSmartThing(i);
		}
		p = p->link;
		i++;
	}
}
int SensorManager::GetTempLen() {
	return tempLen;
}
int SensorManager::GetHumdLen() {
	return humdLen;
}
int SensorManager::GetLuxLen() {
	return luxLen;
}
int SensorManager::GetMotionLen() {
	return motionLen;
}