/*----------------------------------------------------------------------------------------------
���ϸ� : Manager.cpp
�� �� : �������� ó������
������ : gcc -o Manager Manager.cpp -lpthread
���࿹ : sudo ./Manager
----------------------------------------------------------------------------------------------*/
#include "Manager.h"
pthread_mutex_t mutexLock;
int main(int argc, char *argv[]) {

	int count = 0;
	Manager manager(5100);
	manager.ConnectSetting();
	manager.CommuniSetting();
	manager.SetClientLen(manager.GetClientAddr());
	pthread_t manageId;
	pthread_t communiId;
	if (pthread_mutex_init(&mutexLock, NULL) != 0){
		perror("mutex init fail");
		exit(1);
	}
	pthread_create(&manageId, NULL, Manager::CallBackConnect, &manager);
	
	pthread_create(&communiId, NULL, Manager::CallBackCommuniConnect, &manager);
	
	pthread_join(manageId, NULL);
	pthread_join(communiId, NULL);
	return 0;
}


