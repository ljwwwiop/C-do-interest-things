#include "redisDb.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 这个main 主要实现一下redisDb中的一些方法
// 都是server端的功能

#define HASHSIZE 10

// 定义哈希表进行相关操作的函数集
// 计算哈希值的函数
unsigned int myHashFunction(const char *key,int size) {
	char* charkey = (char*)key;
	unsigned int hash = 0;
	for (; *charkey; ++charkey) {
		hash = hash * 33 + *charkey;
	}
	return hash % size;
}

// 复制键的函数
void *myKeyDup(void *key)
{
	return key;
}

// 复制值的函数
void *myValDup(void *obj)
{
	return obj;
}

// 对比键值的函数
int myKeyCompare(void *key1, void *key2) {
	char*charkey1 = (char*)key1;
	char*charkey2 = (char*)key2;
	return strcmp(charkey1, charkey2);
}

//销毁键的函数
void myKeyDestructor(void *key) {
	//free(key);
}
//销毁值的函数
void myValDestructor(void *obj) {
	//free(obj);
}

// 初始化一个数据库
redisDb* redisDbCreate(dictType*type, int hashSize, int id) {
	redisDb*db = (redisDb*)malloc(sizeof(redisDb));
	db->dict = dictCreate(type, hashSize);
	db->id = id;
	return db;
}

// 初始化一个数据库
redisDb* redisDbCreate(dictType* type,int hashsize) {
	redisDb* db = (redisDb*)malloc(sizeof(redisDb));
	db->dict = dictCreate(type, hashsize);
	db->id = NULL;
	printf("Lian Redis WARNING : redisDb's Id is NULL.\n");
	return db;
}

// 向数据库中添加/更新键值
bool redisDbInsert(redisDb* db,char* key,char* val)
{
	int flag = dictInsert(db->dict, key, val);
	if (flag) {
		printf("Lian Redis : redisDb insert/refresh success.\n");
		return true;
	}
	else {
		printf("Lian Redis : redisDb insert/refresh fail.\n");
		return false;
	}
}
//从数据库中删除键
void redisDbDelete(redisDb*db, char* key) {
	dictDelete(db->dict, key);
	printf("Lian Redis : redisDb delete OK.\n");
	return;
}

//从数据库中获取给定key对应的value
char* redisDbFetchValue(redisDb*db, char* key) {
	char* result = dictFetchValue(db->dict, key);
	if (result == "") {
		printf("Lian Redis WARNING: redisDb fetch NULL.\n");
	}
	else {
		printf("Lian Redis : redisDb fetch success.\n");
	}
	return result;
}

//释放数据库
void redisDbRelease(redisDb*db) {
	if (db == NULL)return;
	dictRelease(db->dict);
	free(db);
}


//int main()
//{
//	// 设定一些指定函数
//	dictType* type = (dictType*)malloc(sizeof(dictType));
//	type->hashFunction = myHashFunction;
//
//
//	return 0;
//}
