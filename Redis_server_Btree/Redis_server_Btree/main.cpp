// server 端引入B tree 和 hash 启动index
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

//服务端数据库数组的个数
#define dbNum 15
//端口号                    
#define PORT           5150    
//发送消息的最大字节长度
#define MSGSIZE        1024   
#pragma comment(lib, "ws2_32.lib")  

// 最大线程数
#define MAXCLIENTNUM 15 

// 服务端需要一个存放资源的db数组，每一个客户端一个连接
// 创建一个client，并且让它与对应的数据库db[i]绑定
// db中是一个字典构成的数组，让对应的client存放key-value
// 客户端输入命令，服务端对命令解析，在自己的命令进行对应的运行
// 集合中查询命令是否合法后，相应的命令
// 客户端执行set命令，则是将k-v存储在db的字典中，最后返回响应给客户端
typedef void(*CommandFun)(Server*, Client*, string, string&);
typedef map<string, CommandFun> mymap;
typedef BTree myDB2;

// 创建服务器
Server* server = new Server();

// 分割字符串的方法
vector<string> split(string str,string ch) {
	vector<string> vec;
	if (str == "") return vec;
	string strs = str + ch;

	size_t pos = strs.find(ch);
	size_t size = strs.size();

	// npos为结束的位置
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

// get命令
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

// set命令
void setCommand(Server*server, Client*client, string key, string&value)
{
	//client->db.insert(pair<string,string>(key,value));
	//需要将key进行hash转成int
	int k = DJBHash(key);
	client->db->insert(k, value);
}

// 初始化数据库
void initDB()
{
	cout << "Redis by Lian start-up..." << endl;
}

// 初始化命令库
void initCommand(Server*&server) {
	(server->Commands).insert(pair<string, CommandFun>("set", &setCommand));

	(server->Commands).insert(pair<string, CommandFun>("get", &getCommand));
}

// 初始化服务端
void initServer(Server*&server)
{
	//创建客户端的套接字
	server->sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//初始化数据库
	initDB();

	//初始化命令库
	initCommand(server);

	//绑定
	(server->local).sin_family = AF_INET;
	(server->local).sin_port = htons(PORT);
	(server->local).sin_addr.s_addr = htonl(INADDR_ANY);
	bind(server->sListen, (struct sockaddr *) &(server->local), sizeof(SOCKADDR_IN));

	//监听
	//1表示等待连接队列的最大长度
	listen(server->sListen, 1);
}

// 创建线程
DWORD WINAPI WorkerThread(LPVOID lpParam)
{
	char szMessage[MSGSIZE];
	int ret;
	Client *c = (Client*)lpParam;
	while (true)
	{
		// ret是发送消息的字节长度
		// szMessage是发送消息的内容
		ret = recv(c->sClient,szMessage,MSGSIZE,0);
		szMessage[ret] = '\0';
		// recv(c->sClient, szMessage, MSGSIZE, 0);
		printf("Received [%d bytes]: '%s' \n",  ret,szMessage);

		// 类型转换
		string message(szMessage);
		// 对用户发送的消息进行判断
		if (message == "-v" || message == "--version") {
			const char *str = "Redis by @Lian v=0.0.1000 <00000000/0> 64 bit";
			send(c->sClient,str,strlen(str)+sizeof(char),NULL);
			cout << "命令成功执行，并且向客户端发送了 版本信息" << endl;
		}
		else if (message == "-h"||message == "--help") {
			const char *str = "help document [尚未完善]";
			send(c->sClient, str, strlen(str) + sizeof(char), NULL);
			cout << "命令成功执行，并且向客户端发送了 帮助文档信息" << endl;
		}
		else {
			// 对发送的消息进行分割，查看是否符合get/set命令的格式
			string ch = "-";
			vector<string> vec = split(message, ch);

			// 执行set命令
			if (vec[0] == "set") {
				// 符合命令格式
				if (vec.size() == 3) {
					// 获得键值
					string key = vec[1];
					string value = vec[2];
					cout << vec[0] << " " << vec[1] << " " << vec[2] << endl;

					mymap::iterator it;
					it = server->Commands.find("set");
					if(it == server->Commands.end()) cout << "can't find" << endl;
					else {
						CommandFun cmd = it->second;
						cmd(server, c, key, value);
						// 向客户端发送消息
						const char *str = "set ok";
						send(c->sClient, str, strlen(str) + sizeof(char), NULL);
						cout << "命令成功执行，并且向客户端发送了 set ok" << endl;
					}

				}
				else {
					const char *str = "error ERR wrong number of arguments for 'set' command";
					send(c->sClient, str, strlen(str) + sizeof(char), NULL);
					cout << "向客户端发送 (error) ERR wrong number of arguments for 'set' command" << endl;
				}
				// 执行get命令
			}
			else if (vec[0] == "get") {
				// 符合命令格式
				if (vec.size() == 2) {
					cout << "ok" << endl;
					// 获取键值
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
						// 向客户端发送数据
						char *str = new char[strlen(value.c_str())];
						strcpy(str, value.c_str());
						send(c->sClient,str,strlen(str) + sizeof(char),NULL);
						cout << "命令执行成功，并且向客户端发送了" << value << endl;

					}
				}
				else {
					// 不合格的命令格式
					const char *str = "(error) ERR wrong number of arguments for 'get' command";
					send(c->sClient, str, strlen(str) + sizeof(char), NULL);
					cout << "向客户端发送 (error) ERR wrong number of arguments for 'get' command" << endl;
				}
			}
			else
			{
				// 响应客户端
				const char *str = "(error) ERR no such a command";
				send(c->sClient, str, strlen(str) + sizeof(char), NULL);
				cout << "向客户端发送 (error) ERR no such a command" << endl;
			}

		}


	}
	return 0;

}


int main()
{
	WSADATA wsaData;
	int iaddrSize = sizeof(SOCKADDR_IN);
	// 创建线程
	DWORD dwThreadId;

	//初始化Windows套接字库
	WSAStartup(0x0202, &wsaData);

	//初始化服务端
	initServer(server);

	//获得线程的句柄
	HANDLE hThread[MAXCLIENTNUM];

	for (int i = 0; i < MAXCLIENTNUM; i++)
	{
		// 等待客户端连接
		Client* client = new Client();
		client->sClient = accept(server->sListen,(struct sockaddr *) &(client->my_client),&iaddrSize);
		client->db = new myDB2(3);
		printf("Accepted client:%s:%d\n", inet_ntoa((client->my_client).sin_addr), ntohs((client->my_client).sin_port));
		// 将客户端与服务端中对应的数据库连接
		server->DB2[i] = client->db;

		// 创建工作线程
		hThread[i] = CreateThread(NULL,0,WorkerThread,client,0,NULL);

	}
	WaitForMultipleObjects(2,hThread,TRUE,INFINITE);
	
	// 关闭套接字
	closesocket(server->sListen);
	cout << "线程全部执行完毕" << endl;

	return 0;
}

