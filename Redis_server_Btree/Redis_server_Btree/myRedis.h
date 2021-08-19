#pragma once
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <map>

using namespace std;

// ���ݿ�ĸ���
#define dbNum 15

//������redis����Ҫʹ�õ������ݽṹ
//ʹ��STL�е�map��Ϊdict�����ڿ��ܻ��Լ������� 
/*
	ʹ��map ��һ���ṹ��������Ӧƥ��
*/
//���ݿ�ʹ��һ��dict��ʾ

typedef map<string, string> myDB;
class Server;
class Client;
// ������ĺ���ָ�����typedef
// ������ĺ���ָ���ʾһ�������
typedef void(*CommandFun)(Server* ,Client*,string,string&);

// �����
class Server {
public:
	SOCKET sListen;
	SOCKADDR_IN local;
	// �˿ں�
	int port;
	// ����˴洢��Դ�����ݿ�
	// ���ݿ����飬ÿ��Ԫ����һ��myDB���͵�ָ��
	myDB* DB[dbNum];
	//�����
	//��һ��map��ʾ�洢�ڷ���˵������
	//keyΪ��������֣�valueΪһ��ָ��
	//����ָ�룬�������������ָ���Ӧ�ĺ���
	map<string, CommandFun> Commands;

};


// �ͻ���
class Client {
public:
	SOCKET sClient;
	SOCKADDR_IN my_client;
	// �ͻ��˶�Ӧ�����ݿ�
	myDB db;

};
