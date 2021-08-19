#pragma once

#include <string>
using namespace std;

// ʵ��һ��redis�е�dict
// ��ϣ��Ľڵ�ʹ��dictEntry�ṹ����ʾ
// ÿ��dictEntry�ṹ��������һ��key - value
typedef struct dictEntry {
	// key
	char* key;
	// value
	char* value;
	// �����麯����ָ�룬ָ����һ����ϣ��ڵ㣬�γ������Ա���key��ͻ��
	struct dictEntry *next;
}dictEntry;

// ����һ�����ڲ����ض����ͼ�ֵ�Եĺ���
typedef struct dictType {
	// �����ϣֵ�ĺ���
	unsigned int(*hashFunction)(char* key, int size);
	// ���Ƽ��ĺ���
	char* (*keyDup)(char* key);
	// ����ֵ�ĺ���
	char* (*valDup)(char* obj);
	// �Աȼ��ĺ���
	int (*keyCompare)(char* key1, char *key2);
	// ����ֵ����
	void(*valDestructor)(char* obj);
	//���ټ��ĺ���
	void(*keyDestructor)(char* key);

}dictType;


// ��ϣ��
typedef struct dictht {
	// ��ϣ������
	dictEntry **table;
	// ��ϣ���С
	int size;
	// �ù�ϣ�����нڵ������
	int used;

}dictht;


// �ֵ�
//��ʵ�ֵ���Ƕ���ͨ�Ĺ�ϣ������һ���װ
//������һЩ����
typedef struct dict {
	// �����ض�����
	dictType *type;
	// ��ϣ��
	dictht *ht;

}dict;

// ����һ���µ��ֵ�
dict *dictCreate(dictType *type, int hashSize);
// ����keyѰ������hashTable�ж�Ӧ�Ľڵ�
dictEntry* lookup(dict *d,char* key);
//�������ļ�ֵ����ӵ��ֵ�����
//�������ļ�ֵ����ӵ��ֵ����棬������Ѿ��������ֵ䣬
//��ô����ֵȡ��ԭ�е�ֵ
bool dictInsert(dict *d,char* key,char* val);
// ���ظ����ļ�ֵ
char* dictFetchValue(dict *d,char* key);
//���ֵ���ɾ������������Ӧ�ļ�ֵ��
void dictDelete(dict *d, char* key);
//�ͷŸ����ֵ䣬�Լ��ֵ��а��������м�ֵ��
void dictRelease(dict *d);



