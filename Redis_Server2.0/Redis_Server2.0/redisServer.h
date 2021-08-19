#pragma once
#include <WinSock2.h>
#include "redisDb.h"

typedef struct redisServer {
	SOCKET sListen;
	SOCKADDR_IN local;
	int port;
	// db ���鱣�������������������ݿ�
	redisDb **db;
	int dbnum;

}redisServer;

//�ͷŷ����
void redisServerRelease(redisServer*SERVER) {
	for (int i = 0; i < SERVER->dbnum; i++) {
		//if(SERVER->db[i]->dict->ht->table[0]==NULL)
		//printf("%d\n",i);

		redisDbRelease(SERVER->db[i]);
	}
	//free(server->db);
	//free(server);
}

