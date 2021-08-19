#pragma once
#include "dict.h"
#include <string>

using namespace std;
// ���ݿ�
typedef struct redisDb {
	// ���ݿ��ڷ���˶�Ӧ��id
	int id;
	// ���ݿ�������������ݵ��ֵ�����
	// �ֵ������е�hashTable�������key-v��
	dict *dict;

}redisDb;

// ��ʼ��һ�����ݿ�
redisDb* redisDbCreate(dictType* type,int hashSzie,int id);

// ��ʼ��
redisDb* redisDbCreate(dictType* type,int hashSize);

// �����ݿ������/���¼�ֵ
bool redisDbInsert(redisDb* db,char* key,char* val);

// �����ݿ���ɾ��key
void redisDbDelete(redisDb* db,char* key);

// �����ݿ��ȡ����key��Ӧ��value
char* redisDbFetchValue(redisDb*db, char* key);

// �ͷ����ݿ�
void redisDbRelease(redisDb*db);

