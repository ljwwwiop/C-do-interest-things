// ����server �� client ������ main
#pragma comment(lib, "ws2_32.lib")  // ���ָ��
#pragma warning(disable: 4786)
#pragma warning(disable: 4996)
#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <process.h>
#include <vector>
#include "redisClient.h"
#include "redisServer.h"

using namespace std;

//��������ݿ����������������У�16�����ݿⶼ�Ǹ�ͬһ���û�ʹ�õ�
#define DBNUM 16
//�˿ں�                    
#define PORT 5155    
//������Ϣ������ֽڳ���
#define MSGSIZE 1024 
#define MAXCLIENTNUM 10

//��ϣ��Ĵ�С
#define HASHSIZE 10

// server ��
redisServer *server;
// client 
redisClient *client;

// �����ָ������split����
vector<string> split(string str,string ch)
{
	vector<string> vec;
	if (str == "") {
		return vec;
	}
	string strs = str + ch;

	size_t pos = strs.find(ch);
	size_t size = strs.size();

	// npos����λ��
	while (pos != string::npos) {
		string x = strs.substr(0, pos);
		vec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(ch);
	}
	return vec;
}

// ����һЩ��ϣ��ķ���
// �����ϣֵ
unsigned int myHashFunction(char* key,int size)
{
	// 
	unsigned int hash = 0;
	for (; *key; ++key) hash = hash * 33 + *key;
	return hash % size;
}

// ���Ƽ��ĺ���
char* myKeyDup(char *key)
{
	return key;
}

// ����ֵ�ĺ���
char* myValDup(char* val)
{
	return val;
}

//�Աȼ��ĺ���
int myKeyCompare(char* key1, char* key2) {
	//char*charkey1=(char*)key1.data();
	//char*charkey2=(char*)key2.data();

	return strcmp(key1, key2);
}

//���ټ��ĺ���
void myKeyDestructor(char* key) {
	//key=NULL;
	//free(key);
}
//����ֵ�ĺ���
void myValDestructor(char* obj) {
	//obj=NULL;
	//free(obj);
}

//�����߳�
DWORD WINAPI WorkerThread(LPVOID lpParam) {
	char szMessage[MSGSIZE];
	int ret;
	redisClient *client = (redisClient*)lpParam;
	client->db->dict;
	while (true)
	{
		// ret ������Ϣ���ֽڳ���
		// szMessage��Ϣ����
		ret = recv(client->sClient,szMessage,MSGSIZE,0);
		szMessage[ret] = '\0';
		printf("Lian Redis : Received [%d bytes]: '%s'\n", ret, szMessage);

		// ����ת��
		string message(szMessage);

		// ���û����͵���������ж�
		// �û�����汾��Ϣ
		if (message == "-v" || message =="--version") {
			char *str = "Redis by @Lian v = 0.0.2000<00000000/0> 64 bit";
			// ���û������������Ӧ
			send(client->sClient,str,strlen(str)+sizeof(char),NULL);
			printf("Lian Redis : Send version information success.\n");
		}
		else if (message == "-h" || message == "--help") {
			char *str = "help document not yet perfect";
			// ���û������������Ӧ
			send(client->sClient, str, strlen(str) + sizeof(char), NULL);
			printf("Lian Redis : Send help document success.\n");
		}
		else if (message == "bye" || message == "exit") {
			char *str = "Bye Bye~";
			send(client->sClient, str, strlen(str) + sizeof(char), NULL);
			printf("Lian Redis : Send exit success and loss connect.\n");
			break;
			
		}
		else { // �����������
			string ch = " ";
			vector<string>vec = split(message, ch);
			// ����������ж�
			if (vec[0] == "set") {
				// ��鳤���Ƿ�Ϸ�
				if (vec.size() == 3) {
					// �Ϸ�
					vector<string>::iterator it;
					it = vec.begin();
					it++;
					string key = *it;
					it++;
					string value = *it;
					char *charkey = (char*)key.data();
					char*charvalue = (char*)value.data();
					// ִ��set����
					bool flag = redisClientInsertDB(client,charkey,charvalue);
					// ִ���Ƿ�ɹ�
					if (flag) {
						char *str1 = "set ok";
						send(client->sClient,str1,strlen(str1)+sizeof(char),NULL);
						printf("Lian Redis :Send Set Ok.\n");
					}
					else {
						char *str2 = "set error";
						send(client->sClient, str2, strlen(str2) + sizeof(char), NULL);
						printf("Lian Redis :Send Set Error.\n");
					
					}

				}
				else
				{
					// size ���Ϸ���
					char *str = "Lian Redis Error: Wrong size of arguments for 'set' command";
					send(client->sClient, str, strlen(str) + sizeof(char), NULL);
					printf("Lian Redis :Send Set Error.\n");
				}

			}
			else if (vec[0] == "get") {
				// ����get����
				if (vec.size() == 2) {
					// ��ȡkey
					vector<string>::iterator it;
					it = vec.begin();
					it++;
					string key = *it;
					char*charkey = (char*)key.data();
					char* getValue = redisClientFetchDB(client, charkey);
					printf("Lian Redis : ---> %s\n", getValue);
					// ���û�ҵ����key - v
					if (getValue == "") {
						char *str = "Lian Redis ERROR : Can't find such key-value pairs";
						send(client->sClient, str, strlen(str) + sizeof(char), NULL);
						printf("Lian Redis : Send get error\n");
					}
					else {
						send(client->sClient, getValue, strlen(getValue) + sizeof(char), NULL);
						printf("Lian Redis : Send get ok !\n");
					}

				}
				else {
					// ���Ȳ���
					char *str = "Lian Redis ERROR : Wrong number of arguments for 'get' command";
					send(client->sClient, str, strlen(str) + sizeof(char), NULL);
					printf("Lian Redis : Send get error !\n");

				}
			}
			else {
				// û�е�����
				char *str = "Lian Redis ERROR : No such command";
				send(client->sClient, str, strlen(str) + sizeof(char), NULL);
				printf("Lian Redis : Send command error.\n");
			}
		}

	}
	return 0;

}

//int main()
//{	
//	// main
//	dictType*type = (dictType*)malloc(sizeof(dictType));
//	type->hashFunction = myHashFunction;
//	type->keyCompare = myKeyCompare;
//	type->keyDup = myKeyDup;
//	type->valDup = myValDup;
//	type->keyDestructor = myKeyDestructor;
//	type->valDestructor = myValDestructor;
//
//	WSADATA wsaData;
//	int iaddrSize = sizeof(SOCKADDR_IN);
//	DWORD dwThread;
//
//	// ��ʼ��windows�׽��ֿ�
//	WSAStartup(0x0202, &wsaData);
//
//	// ��ʼ�������
//	server = (redisServer*)malloc(sizeof(redisServer));
//	// �����ͻ��˵��׽���
//	server->sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//
//	// ��
//	(server->local).sin_family = AF_INET;
//	(server->local).sin_port = htons(PORT);
//	(server->local).sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//	bind(server->sListen,(struct sockaddr *)&(server->local),sizeof(SOCKADDR_IN));
//
//	server->dbnum = DBNUM;
//
//	// ��ʼ�����ݿ�����
//	server->db = (redisDb**)malloc(sizeof(redisDb*)*server->dbnum);
//	for (int k = 0; k < server->dbnum; k++)
//		server->db[k] = redisDbCreate(type,HASHSIZE,k);
//	
//	// ����
//	listen(server->sListen, 1);
//
//	// ��ȡ�߳̾��
//	HANDLE hThread[MAXCLIENTNUM];
//
//	printf("Lian Redis Server -- v0.0.0.2000.\n\n");
//
//	for (int i = 0; i < MAXCLIENTNUM;i++) {
//		// �ȴ��ͻ�������
//		redisClient *client = redisClientCreate();
//		client->sClient = accept(server->sListen,(struct sockaddr *) &(client->my_client),&iaddrSize);
//		// ���ͻ��˶Է�����ж�Ӧ�����ݿ����ӣ�Ĭ�ϴӵ�0����ʼ
//		client->db = server->db[0];
//		printf("Welcome to Lian Redis Server.\n");
//		printf("Accepted client:%s:%d\n", inet_ntoa((client->my_client).sin_addr),ntohs((client->my_client).sin_port));
//		// ���������߳�
//		hThread[i] = CreateThread(NULL,0,WorkerThread,client,0,NULL);
//	
//	}
//	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
//
//	// �ر��׽���
//	closesocket(server->sListen);
//	printf("Lian Redis : Threads are fully executed\n");
//
//	redisServerRelease(server);
//
//	system("pause");
//	return 0;
//}