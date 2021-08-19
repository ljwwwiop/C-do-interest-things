#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <string>
#pragma warning(disable: 4996)
using namespace std;

//���������ʹ�õĳ���      
#define SERVER_ADDRESS "127.0.0.1" //��������IP��ַ      
#define PORT           5155         //�������Ķ˿ں�      
#define MSGSIZE        1024         //�շ��������Ĵ�С      
#pragma comment(lib, "ws2_32.lib")  

int main()
{
	WSADATA wsaData;
	// ���������׽���
	SOCKET sClient;
	// �����ַ��Ϣ
	SOCKADDR_IN server;
	// �շ�������
	char szMessage[MSGSIZE];
	// �ɹ������ֽڵĸ���
	int ret;

	// init windows socket library
	WSAStartup(0x0202, &wsaData);

	// �����ͻ��˵��׽���
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // ָ��Э��

	// ָ��Զ�̷�������ַ��Ϣ(PORT,IP��ַ)
	memset(&server,0,sizeof(SOCKADDR_IN)); // ����server Ϊ0
	server.sin_family = PF_INET;
	server.sin_port = htons(PORT); // ָ�����ӵ�port,htons()���� ת�����port��Ϊbytes
	server.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);

	// ���ӷ����
	connect(sClient,(struct sockaddr *) &server,sizeof(SOCKADDR_IN)); //ֱ��return sClient�ϣ�����ʹ��sClient��ʹ������
	printf("Redis by @LianWJ start-up...\n");
	while (true) {
		printf("Lian_Redis>");

		// ��ȡ�������� ����ʹ��scanf()
		cin.getline(szMessage,20); //The gets() functionreads characters from stdin and loads them into szMessage  
		send(sClient,szMessage,strlen(szMessage),0); // ��������
		Sleep(1000);

		// ���ܷ���˷��ص�����
		char szBuffer[MAXBYTE] = { 0 };
		recv(sClient,szBuffer,MAXBYTE,NULL);
		// ����յ�������
		cout << "�������ظ���" << szBuffer << endl;


	}
	// �ͷ�����
	closesocket(sClient);
	WSACleanup();

	return 0;
}


