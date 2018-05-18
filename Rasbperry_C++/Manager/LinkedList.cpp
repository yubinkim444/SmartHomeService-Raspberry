
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
// ����Ʈ�� �ʱ�ȭ�Ѵ�.
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
// phead: ����Ʈ�� ��� �������� ������
// p : ���� ���
// new_node : ���Ե� ��� 
void SensorManager::insertSmartThing(SmartThing *p, SmartThing *new_node)
{
	if (*phead == NULL) {	// ���鸮��Ʈ�� ���
		new_node->link = NULL;
		*phead = new_node;
	}
	else if (p == NULL) { // p�� NULL�̸� ù��° ���� ����
		new_node->link = *phead;
		*phead = new_node;
	}
	else {			 // p ������ ����
		new_node->link = p->link;
		p->link = new_node;
	}
}
// phead : ��� �����Ϳ� ���� ������ 
// p: ������ ����� ���� ���
// removed: ������ ��� 
void SensorManager::removeSmartThing(SmartThing *p, SmartThing *removed)
{
	if (p == NULL)
		*phead = (*phead)->link;
	else
		p->link = removed->link;
	free(removed);
}

// ����Ʈ�ȿ��� pos ��ġ�� ��带 ��ȯ�Ѵ�.
SmartThing * SensorManager::getSmartThingAt(int pos)
{
	int i;
	SmartThing *tmp_node = head;
	if (pos < 0) return NULL;
	for (i = 0; i<pos; i++)
		tmp_node = tmp_node->link;
	return tmp_node;
}
// ����Ʈ�� �׸��� ������ ��ȯ�Ѵ�.
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
// �־��� ��ġ�� �����͸� �����Ѵ�.
void SensorManager::add(int position, SmartThing* newThing)
{
	SmartThing *p;
	if ((position >= 0) && (position <= length)) {
		p = getSmartThingAt(position - 1);
		insertSmartThing(p, newThing);
		length++;
	}
}
// ����Ʈ�� ���� �����͸� �����Ѵ�.
void SensorManager::addLast(SmartThing* newThing)
{
	add(getLength(), newThing);
}
// ����Ʈ�� ���� �����͸� �����Ѵ�.
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

// �־��� ��ġ�� �����͸� �����Ѵ�.
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
// ������ ������ ����Ѵ�. 
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
// ������ ���� s�� ��带 ã�´�.
SmartThing* SensorManager::isInList(int type, char ips[])
{
	SmartThing *p;
	p = head; 	// ��� �����Ϳ������� �����Ѵ�.
	while ((p != NULL)) {
		// ����� �����Ͱ� item�̸�
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