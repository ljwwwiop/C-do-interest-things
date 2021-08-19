// server ������B tree �� hash ����index
#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <Windows.h>
#include <process.h>
#include <malloc.h>
#include <map>
#include <vector>
#include "myRedis_BTree.h"

using namespace std;

//��������ݿ�����ĸ���
#define dbNum 15
//�˿ں�                    
#define PORT           5150    
//������Ϣ������ֽڳ���
#define MSGSIZE        1024   
#pragma comment(lib, "ws2_32.lib")  

// ����߳���
#define MAXCLIENTNUM 15 

// �������Ҫһ�������Դ��db���飬ÿһ���ͻ���һ������
// ����һ��client�������������Ӧ�����ݿ�db[i]��
// db����һ���ֵ乹�ɵ����飬�ö�Ӧ��client���key-value
// �ͻ��������������˶�������������Լ���������ж�Ӧ������
// �����в�ѯ�����Ƿ�Ϸ�����Ӧ������
// �ͻ���ִ��set������ǽ�k-v�洢��db���ֵ��У���󷵻���Ӧ���ͻ���
typedef void(*CommandFun)(Server*, Client*, string, string&);
typedef map<string, CommandFun> mymap;
typedef BTree myDB2;

// ����������
Server* server = new Server();

// �ָ��ַ����ķ���
vector<string> split(string str,string ch) {
	vector<string> vec;
	if (str == "") return vec;
	string strs = str + ch;

	size_t pos = strs.find(ch);
	size_t size = strs.size();

	// nposΪ������λ��
	while (pos!=string::npos) {
		string x = strs.substr(0, pos);
		vec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(ch);
	}
	return vec;
}

int DJBHash(string str)
{
	// mod
	unsigned int hash = 5381;
	for (int i = 0; i < str.length(); i++)
	{
		hash += (hash << 5) + str[i];
	}
	return (hash & 0x7FFFFFFF) % 1000;
}

// get����
void getCommand(Server*server, Client*client, string key, string&value)
{
	int k = DJBHash(key);
	string ss = client->db->getone(k);
	if (ss == "") {
		cout << "get null" << endl;
	}
	else {
		value = ss;
	}
}

// set����
void setCommand(Server*server, Client*client, string key, string&value)
{
	//client->db.insert(pair<string,string>(key,value));
	//��Ҫ��key����hashת��int
	int k = DJBHash(key);
	client->db->insert(k, value);
}

// ��ʼ�����ݿ�
void initDB()
{
	cout << "Redis by Lian start-up..." << endl;
}

// ��ʼ�������
void initCommand(Server*&server) {
	(server->Commands).insert(pair<string, CommandFun>("set", &setCommand));

	(server->Commands).insert(pair<string, CommandFun>("get", &getCommand));
}

// ��ʼ�������
void initServer(Server*&server)
{
	//�����ͻ��˵��׽���
	server->sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//��ʼ�����ݿ�
	initDB();

	//��ʼ�������
	initCommand(server);

	//��
	(server->local).sin_family = AF_INET;
	(server->local).sin_port = htons(PORT);
	(server->local).sin_addr.s_addr = htonl(INADDR_ANY);
	bind(server->sListen, (struct sockaddr *) &(server->local), sizeof(SOCKADDR_IN));

	//����
	//1��ʾ�ȴ����Ӷ��е���󳤶�
	listen(server->sListen, 1);
}

// �����߳�
DWORD WINAPI WorkerThread(LPVOID lpParam)
{
	char szMessage[MSGSIZE];
	int ret;
	Client *c = (Client*)lpParam;
	while (true)
	{
		// ret�Ƿ�����Ϣ���ֽڳ���
		// szMessage�Ƿ�����Ϣ������
		ret = recv(c->sClient,szMessage,MSGSIZE,0);
		szMessage[ret] = '\0';
		// recv(c->sClient, szMessage, MSGSIZE, 0);
		printf("Received [%d bytes]: '%s' \n",  ret,szMessage);

		// ����ת��
		string message(szMessage);
		// ���û����͵���Ϣ�����ж�
		if (message == "-v" || message == "--version") {
			const char *str = "Redis by @Lian v=0.0.1000 <00000000/0> 64 bit";
			send(c->sClient,str,strlen(str)+sizeof(char),NULL);
			cout << "����ɹ�ִ�У�������ͻ��˷����� �汾��Ϣ" << endl;
		}
		else if (message == "-h"||message == "--help") {
			const char *str = "help document [��δ����]";
			send(c->sClient, str, strlen(str) + sizeof(char), NULL);
			cout << "����ɹ�ִ�У�������ͻ��˷����� �����ĵ���Ϣ" << endl;
		}
		else {
			// �Է��͵���Ϣ���зָ�鿴�Ƿ����get/set����ĸ�ʽ
			string ch = "-";
			vector<string> vec = split(message, ch);

			// ִ��set����
			if (vec[0] == "set") {
				// ���������ʽ
				if (vec.size() == 3) {
					// ��ü�ֵ
					string key = vec[1];
					string value = vec[2];
					cout << vec[0] << " " << vec[1] << " " << vec[2] << endl;

					mymap::iterator it;
					it = server->Commands.find("set");
					if(it == server->Commands.end()) cout << "can't find" << endl;
					else {
						CommandFun cmd = it->second;
						cmd(server, c, key, value);
						// ��ͻ��˷�����Ϣ
						const char *str = "set ok";
						send(c->sClient, str, strlen(str) + sizeof(char), NULL);
						cout << "����ɹ�ִ�У�������ͻ��˷����� set ok" << endl;
					}

				}
				else {
					const char *str = "error ERR wrong number of arguments for 'set' command";
					send(c->sClient, str, strlen(str) + sizeof(char), NULL);
					cout << "��ͻ��˷��� (error) ERR wrong number of arguments for 'set' command" << endl;
				}
				// ִ��get����
			}
			else if (vec[0] == "get") {
				// ���������ʽ
				if (vec.size() == 2) {
					cout << "ok" << endl;
					// ��ȡ��ֵ
					string key = vec[1];
					string value = "";
					cout << vec[0] << " " << vec[1] << endl;

					mymap::iterator it;
					it = server->Commands.find("get");
					if (it == server->Commands.end())
					{
						cout << "can't find" << endl;
					}
					else {
						CommandFun cmd = it->second;
						cmd(server, c, key, value);
						// ��ͻ��˷�������
						char *str = new char[strlen(value.c_str())];
						strcpy(str, value.c_str());
						send(c->sClient,str,strlen(str) + sizeof(char),NULL);
						cout << "����ִ�гɹ���������ͻ��˷�����" << value << endl;

					}
				}
				else {
					// ���ϸ�������ʽ
					const char *str = "(error) ERR wrong number of arguments for 'get' command";
					send(c->sClient, str, strlen(str) + sizeof(char), NULL);
					cout << "��ͻ��˷��� (error) ERR wrong number of arguments for 'get' command" << endl;
				}
			}
			else
			{
				// ��Ӧ�ͻ���
				const char *str = "(error) ERR no such a command";
				send(c->sClient, str, strlen(str) + sizeof(char), NULL);
				cout << "��ͻ��˷��� (error) ERR no such a command" << endl;
			}

		}


	}
	return 0;

}


int main()
{
	WSADATA wsaData;
	int iaddrSize = sizeof(SOCKADDR_IN);
	// �����߳�
	DWORD dwThreadId;

	//��ʼ��Windows�׽��ֿ�
	WSAStartup(0x0202, &wsaData);

	//��ʼ�������
	initServer(server);

	//����̵߳ľ��
	HANDLE hThread[MAXCLIENTNUM];

	for (int i = 0; i < MAXCLIENTNUM; i++)
	{
		// �ȴ��ͻ�������
		Client* client = new Client();
		client->sClient = accept(server->sListen,(struct sockaddr *) &(client->my_client),&iaddrSize);
		client->db = new myDB2(3);
		printf("Accepted client:%s:%d\n", inet_ntoa((client->my_client).sin_addr), ntohs((client->my_client).sin_port));
		// ���ͻ����������ж�Ӧ�����ݿ�����
		server->DB2[i] = client->db;

		// ���������߳�
		hThread[i] = CreateThread(NULL,0,WorkerThread,client,0,NULL);

	}
	WaitForMultipleObjects(2,hThread,TRUE,INFINITE);
	
	// �ر��׽���
	closesocket(server->sListen);
	cout << "�߳�ȫ��ִ�����" << endl;

	return 0;
}

