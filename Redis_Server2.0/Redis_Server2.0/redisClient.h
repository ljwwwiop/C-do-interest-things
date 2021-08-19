#include "redisDb.h"
#include <stdio.h>
#include <WinSock2.h>
#include "redisServer.h"

typedef struct redisClient {
	SOCKET sClient;
	SOCKADDR_IN my_client;
	// ��¼��ǰ�û�����ʹ�õ����ݿ�
	redisDb*db;

}redisClient;

// ��ʼ���ͻ���
redisClient* redisClientCreate() {
	redisClient* client = (redisClient*)malloc(sizeof(redisClient));
	return client; 
}

//  �л����ݿ�Ĳ���
bool redisClientChangeDB(redisServer *server,int dbId,redisClient*&client)
{
	// �ж��л������ݿ�ID�Ƿ�Ϸ�
	if (dbId >= server->dbnum || dbId < 0)
	{
		printf("Lian Redis Error:DbId is Illegal.\n");
		return false;
	}
	client->db = server->db[dbId];
	return true;
}

// ��ǰʹ�õ����ݿ��в��� or ����
bool redisClientInsertDB(redisClient *client,char *key,char* val)
{
	// ֱ�ӽ���redisDb ����
	return redisDbInsert(client->db, key, val);
}

// ��ǰʹ�õ����ݿ���ɾ��ָ����key��Ӧ��value
void redisClientDeleteDB(redisClient *client,char *key) 
{
	redisDbDelete(client->db,key);
	return;
}

// ��ǰʹ�õ����ݿ��л�ȡָ��key��Ӧ��value
char* redisClientFetchDB(redisClient *client,char *key)
{
	return redisDbFetchValue(client->db,key);
}


