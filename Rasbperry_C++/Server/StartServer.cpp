/*----------------------------------------------------------------------------------------------
���ϸ� : Manager.cpp
�� �� : �������� ó������
������ : gcc -o Manager Manager.cpp -lpthread
���࿹ : sudo ./Manager
----------------------------------------------------------------------------------------------*/
#include "Server.h"
pthread_mutex_t mutexLock;
int main(int argc, char *argv[]) {

	int count = 0;
	Server s(8080);
	s.ConnectSetting();
	s.SetClientLen(s.GetClientAddr());
	pthread_t manageId;
	pthread_t communiId;

	if (pthread_mutex_init(&mutexLock, NULL) != 0){
		perror("mutex init fail");
		exit(1);
	}
	pthread_create(&manageId, NULL, Server::CallBackConnect, &s);


	pthread_join(manageId, NULL);

	return 0;
}


