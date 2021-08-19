#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <string>
#pragma warning(disable: 4996)
using namespace std;

//定义程序中使用的常量      
#define SERVER_ADDRESS "127.0.0.1" //服务器端IP地址      
#define PORT           5155         //服务器的端口号      
#define MSGSIZE        1024         //收发缓冲区的大小      
#pragma comment(lib, "ws2_32.lib")  

int main()
{
	WSADATA wsaData;
	// 连接所有套接字
	SOCKET sClient;
	// 保存地址信息
	SOCKADDR_IN server;
	// 收发缓冲区
	char szMessage[MSGSIZE];
	// 成功接收字节的个数
	int ret;

	// init windows socket library
	WSAStartup(0x0202, &wsaData);

	// 创建客户端的套接字
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 指明协议

	// 指明远程服务器地址信息(PORT,IP地址)
	memset(&server,0,sizeof(SOCKADDR_IN)); // 重置server 为0
	server.sin_family = PF_INET;
	server.sin_port = htons(PORT); // 指明连接的port,htons()用于 转换这个port成为bytes
	server.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);

	// 连接服务端
	connect(sClient,(struct sockaddr *) &server,sizeof(SOCKADDR_IN)); //直接return sClient上，可以使用sClient来使用连接
	printf("Redis by @LianWJ start-up...\n");
	while (true) {
		printf("Lian_Redis>");

		// 获取键盘输入 不能使用scanf()
		cin.getline(szMessage,20); //The gets() functionreads characters from stdin and loads them into szMessage  
		send(sClient,szMessage,strlen(szMessage),0); // 发送数据
		Sleep(1000);

		// 接受服务端返回的数据
		char szBuffer[MAXBYTE] = { 0 };
		recv(sClient,szBuffer,MAXBYTE,NULL);
		// 输出收到的数据
		cout << "服务器回复：" << szBuffer << endl;


	}
	// 释放连接
	closesocket(sClient);
	WSACleanup();

	return 0;
}


