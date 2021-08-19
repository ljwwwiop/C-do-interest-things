#include "redisDb.h"
#include <stdio.h>
#include <WinSock2.h>
#include "redisServer.h"

typedef struct redisClient {
	SOCKET sClient;
	SOCKADDR_IN my_client;
	// 记录当前用户正在使用的数据库
	redisDb*db;

}redisClient;

// 初始化客户端
redisClient* redisClientCreate() {
	redisClient* client = (redisClient*)malloc(sizeof(redisClient));
	return client; 
}

//  切换数据库的操作
bool redisClientChangeDB(redisServer *server,int dbId,redisClient*&client)
{
	// 判断切换的数据库ID是否合法
	if (dbId >= server->dbnum || dbId < 0)
	{
		printf("Lian Redis Error:DbId is Illegal.\n");
		return false;
	}
	client->db = server->db[dbId];
	return true;
}

// 向当前使用的数据库中插入 or 更新
bool redisClientInsertDB(redisClient *client,char *key,char* val)
{
	// 直接接入redisDb 方法
	return redisDbInsert(client->db, key, val);
}

// 向当前使用的数据库中删除指定的key对应的value
void redisClientDeleteDB(redisClient *client,char *key) 
{
	redisDbDelete(client->db,key);
	return;
}

// 向当前使用的数据库中获取指定key对应的value
char* redisClientFetchDB(redisClient *client,char *key)
{
	return redisDbFetchValue(client->db,key);
}


