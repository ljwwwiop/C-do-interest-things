#pragma once
#include <WinSock2.h>
#include "redisDb.h"

typedef struct redisServer {
	SOCKET sListen;
	SOCKADDR_IN local;
	int port;
	// db 数组保存服务器里面的所有数据库
	redisDb **db;
	int dbnum;

}redisServer;

//释放服务端
void redisServerRelease(redisServer*SERVER) {
	for (int i = 0; i < SERVER->dbnum; i++) {
		//if(SERVER->db[i]->dict->ht->table[0]==NULL)
		//printf("%d\n",i);

		redisDbRelease(SERVER->db[i]);
	}
	//free(server->db);
	//free(server);
}

