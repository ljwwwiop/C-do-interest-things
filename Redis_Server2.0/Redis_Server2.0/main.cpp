// 测试server 和 client 主函数 main
#pragma comment(lib, "ws2_32.lib")  // 添加指令
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

//服务端数据库数量——单机版中，16个数据库都是给同一个用户使用的
#define DBNUM 16
//端口号                    
#define PORT 5155    
//发送消息的最大字节长度
#define MSGSIZE 1024 
#define MAXCLIENTNUM 10

//哈希表的大小
#define HASHSIZE 10

// server 端
redisServer *server;
// client 
redisClient *client;

// 用来分割命令的split方法
vector<string> split(string str,string ch)
{
	vector<string> vec;
	if (str == "") {
		return vec;
	}
	string strs = str + ch;

	size_t pos = strs.find(ch);
	size_t size = strs.size();

	// npos结束位置
	while (pos != string::npos) {
		string x = strs.substr(0, pos);
		vec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(ch);
	}
	return vec;
}

// 定义一些哈希表的方法
// 计算哈希值
unsigned int myHashFunction(char* key,int size)
{
	// 
	unsigned int hash = 0;
	for (; *key; ++key) hash = hash * 33 + *key;
	return hash % size;
}

// 复制键的函数
char* myKeyDup(char *key)
{
	return key;
}

// 复制值的函数
char* myValDup(char* val)
{
	return val;
}

//对比键的函数
int myKeyCompare(char* key1, char* key2) {
	//char*charkey1=(char*)key1.data();
	//char*charkey2=(char*)key2.data();

	return strcmp(key1, key2);
}

//销毁键的函数
void myKeyDestructor(char* key) {
	//key=NULL;
	//free(key);
}
//销毁值的函数
void myValDestructor(char* obj) {
	//obj=NULL;
	//free(obj);
}

//工作线程
DWORD WINAPI WorkerThread(LPVOID lpParam) {
	char szMessage[MSGSIZE];
	int ret;
	redisClient *client = (redisClient*)lpParam;
	client->db->dict;
	while (true)
	{
		// ret 返回消息的字节长度
		// szMessage消息内容
		ret = recv(client->sClient,szMessage,MSGSIZE,0);
		szMessage[ret] = '\0';
		printf("Lian Redis : Received [%d bytes]: '%s'\n", ret, szMessage);

		// 类型转换
		string message(szMessage);

		// 对用户发送的请求进行判断
		// 用户请求版本信息
		if (message == "-v" || message =="--version") {
			char *str = "Redis by @Lian v = 0.0.2000<00000000/0> 64 bit";
			// 对用户的请求进行响应
			send(client->sClient,str,strlen(str)+sizeof(char),NULL);
			printf("Lian Redis : Send version information success.\n");
		}
		else if (message == "-h" || message == "--help") {
			char *str = "help document not yet perfect";
			// 对用户的请求进行响应
			send(client->sClient, str, strlen(str) + sizeof(char), NULL);
			printf("Lian Redis : Send help document success.\n");
		}
		else if (message == "bye" || message == "exit") {
			char *str = "Bye Bye~";
			send(client->sClient, str, strlen(str) + sizeof(char), NULL);
			printf("Lian Redis : Send exit success and loss connect.\n");
			break;
			
		}
		else { // 其余操作命令
			string ch = " ";
			vector<string>vec = split(message, ch);
			// 对命令进行判断
			if (vec[0] == "set") {
				// 检查长度是否合法
				if (vec.size() == 3) {
					// 合法
					vector<string>::iterator it;
					it = vec.begin();
					it++;
					string key = *it;
					it++;
					string value = *it;
					char *charkey = (char*)key.data();
					char*charvalue = (char*)value.data();
					// 执行set命令
					bool flag = redisClientInsertDB(client,charkey,charvalue);
					// 执行是否成功
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
					// size 不合法的
					char *str = "Lian Redis Error: Wrong size of arguments for 'set' command";
					send(client->sClient, str, strlen(str) + sizeof(char), NULL);
					printf("Lian Redis :Send Set Error.\n");
				}

			}
			else if (vec[0] == "get") {
				// 进行get操作
				if (vec.size() == 2) {
					// 获取key
					vector<string>::iterator it;
					it = vec.begin();
					it++;
					string key = *it;
					char*charkey = (char*)key.data();
					char* getValue = redisClientFetchDB(client, charkey);
					printf("Lian Redis : ---> %s\n", getValue);
					// 如果没找到这个key - v
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
					// 长度不对
					char *str = "Lian Redis ERROR : Wrong number of arguments for 'get' command";
					send(client->sClient, str, strlen(str) + sizeof(char), NULL);
					printf("Lian Redis : Send get error !\n");

				}
			}
			else {
				// 没有的命令
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
//	// 初始化windows套接字库
//	WSAStartup(0x0202, &wsaData);
//
//	// 初始化服务端
//	server = (redisServer*)malloc(sizeof(redisServer));
//	// 创建客户端的套接字
//	server->sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//
//	// 绑定
//	(server->local).sin_family = AF_INET;
//	(server->local).sin_port = htons(PORT);
//	(server->local).sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//	bind(server->sListen,(struct sockaddr *)&(server->local),sizeof(SOCKADDR_IN));
//
//	server->dbnum = DBNUM;
//
//	// 初始化数据库数组
//	server->db = (redisDb**)malloc(sizeof(redisDb*)*server->dbnum);
//	for (int k = 0; k < server->dbnum; k++)
//		server->db[k] = redisDbCreate(type,HASHSIZE,k);
//	
//	// 监听
//	listen(server->sListen, 1);
//
//	// 获取线程句柄
//	HANDLE hThread[MAXCLIENTNUM];
//
//	printf("Lian Redis Server -- v0.0.0.2000.\n\n");
//
//	for (int i = 0; i < MAXCLIENTNUM;i++) {
//		// 等待客户端连接
//		redisClient *client = redisClientCreate();
//		client->sClient = accept(server->sListen,(struct sockaddr *) &(client->my_client),&iaddrSize);
//		// 将客户端对服务端中对应的数据库连接，默认从第0个开始
//		client->db = server->db[0];
//		printf("Welcome to Lian Redis Server.\n");
//		printf("Accepted client:%s:%d\n", inet_ntoa((client->my_client).sin_addr),ntohs((client->my_client).sin_port));
//		// 创建工作线程
//		hThread[i] = CreateThread(NULL,0,WorkerThread,client,0,NULL);
//	
//	}
//	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
//
//	// 关闭套接字
//	closesocket(server->sListen);
//	printf("Lian Redis : Threads are fully executed\n");
//
//	redisServerRelease(server);
//
//	system("pause");
//	return 0;
//}