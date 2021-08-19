#include "redisDb.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ���main ��Ҫʵ��һ��redisDb�е�һЩ����
// ����server�˵Ĺ���

#define HASHSIZE 10

// �����ϣ�������ز����ĺ�����
// �����ϣֵ�ĺ���
unsigned int myHashFunction(const char *key,int size) {
	char* charkey = (char*)key;
	unsigned int hash = 0;
	for (; *charkey; ++charkey) {
		hash = hash * 33 + *charkey;
	}
	return hash % size;
}

// ���Ƽ��ĺ���
void *myKeyDup(void *key)
{
	return key;
}

// ����ֵ�ĺ���
void *myValDup(void *obj)
{
	return obj;
}

// �Աȼ�ֵ�ĺ���
int myKeyCompare(void *key1, void *key2) {
	char*charkey1 = (char*)key1;
	char*charkey2 = (char*)key2;
	return strcmp(charkey1, charkey2);
}

//���ټ��ĺ���
void myKeyDestructor(void *key) {
	//free(key);
}
//����ֵ�ĺ���
void myValDestructor(void *obj) {
	//free(obj);
}

// ��ʼ��һ�����ݿ�
redisDb* redisDbCreate(dictType*type, int hashSize, int id) {
	redisDb*db = (redisDb*)malloc(sizeof(redisDb));
	db->dict = dictCreate(type, hashSize);
	db->id = id;
	return db;
}

// ��ʼ��һ�����ݿ�
redisDb* redisDbCreate(dictType* type,int hashsize) {
	redisDb* db = (redisDb*)malloc(sizeof(redisDb));
	db->dict = dictCreate(type, hashsize);
	db->id = NULL;
	printf("Lian Redis WARNING : redisDb's Id is NULL.\n");
	return db;
}

// �����ݿ������/���¼�ֵ
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
//�����ݿ���ɾ����
void redisDbDelete(redisDb*db, char* key) {
	dictDelete(db->dict, key);
	printf("Lian Redis : redisDb delete OK.\n");
	return;
}

//�����ݿ��л�ȡ����key��Ӧ��value
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

//�ͷ����ݿ�
void redisDbRelease(redisDb*db) {
	if (db == NULL)return;
	dictRelease(db->dict);
	free(db);
}


//int main()
//{
//	// �趨һЩָ������
//	dictType* type = (dictType*)malloc(sizeof(dictType));
//	type->hashFunction = myHashFunction;
//
//
//	return 0;
//}
