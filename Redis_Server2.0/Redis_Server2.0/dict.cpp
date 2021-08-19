#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

using namespace std;
// �����ϣ��Ĵ�С
#define HASHSIZE 10
/*
// �����ϣ�������ز����ĺ���
// �����ϣֵ�ĺ���
unsigned int myHashFunction(const char *key,int size)
{
	char* charkey = (char *)key;
	unsigned int hash = 0;
	for (; *charkey;++charkey) {
		// hash = hash*p+value
		hash = hash * 33 + *charkey;
	}
	return hash % size;
}

// ���Ƽ��ĺ���
char *myKeyDup(char *key) {
	return key;
}
//����ֵ�ĺ���
char *myValDup(char *obj) {
	return obj;
}
//�Աȼ��ĺ���
int myKeyCompare(const char *key1, char *key2) {
	char*charkey1 = (char*)key1;
	char*charkey2 = (char*)key2;
	return strcmp(charkey1, charkey2);
}
//���ټ��ĺ���
void myKeyDestructor(char *key) {
	free(key);
}
//����ֵ�ĺ���
void myValDestructor(char *obj) {
	free(obj);
}*/

// ����һ���µ��ֵ�
dict *dictCreate(dictType *type,int hashSize)
{
	// �����ڴ��С
	dict* d = (dict*)malloc(sizeof(dict));
	if (d == NULL) {
		printf("Lian Redis Error : Dict malloc fail.\n");
		return NULL;
	}
	// ��hashtable������ز������ض�������
	if(type == NULL) printf("Lian Redis Warning : Type is null.\n");
	d->type = type;
	// ��ʼ����ϣ��
	d->ht = (dictht*)malloc(sizeof(dictht));
	d->ht->size = hashSize;
	d->ht->used = 0;
	d->ht->table = (dictEntry**)malloc(sizeof(dictEntry*)*hashSize);
	// ȫ���ڵ㶼��Ϊnull
	for (int i = 0; i < hashSize; i++)
		d->ht->table[i] = NULL;
	return d;

}

// ����keyѰ������hashTable�ж�Ӧ�Ľڵ�
dictEntry* lookup(dict *d,char* key)
{
	dictEntry* node;
	// ��key��hashtable�ж�Ӧ���±�
	unsigned int index;
	index = d->type->hashFunction(key, d->ht->size);
	if (d->ht->table[index] == NULL) {
		return NULL;
	}
	for (node = d->ht->table[index]; node;node->next) {
		if (!(d->type->keyCompare(key, node->key))) {
			return node;
		}
	}
	return NULL;
}

// �������ļ�ֵ�� - ��ӵ��ֵ�����
bool dictInsert(dict *d, char* key,char* val)
{
	unsigned int index;
	dictEntry* node;
	//������-���в������
	if (!(node = lookup(d, key))) {
		index = d->type->hashFunction(key, d->ht->size);
		node = (dictEntry*)malloc(sizeof(dictEntry));
		if (node == NULL) return false;

		//TODO
		node->key = (char*)malloc(sizeof(char)*(strlen(key) + 1));
		strcpy(node->key, key);
		// ����
		node->next = d->ht->table[index];
		// ��ֵ
		d->ht->table[index] = node;
	}
	//�����ڡ������и��²���,ֱ���޸����Ӧ��valueֵ
	//TODO
	node->value = (char*)malloc(sizeof(char)*(strlen(val) + 1));
	strcpy(node->value, val);

	return true;
}

// ���ظ�����key��value
char* dictFetchValue(dict *d,char* key) {
	dictEntry* node;
	//�Ҳ���������
	if (!(node = lookup(d, key))) {
		return "";
	}
	//cout<<node->value<<"coutcoutcout"<<endl;
	//printf("printf : %s\n",node->value);
	return node->value;
}


// ���ֵ���ɾ������key��Ӧ��value
void dictDelete(dict *d, char* key)
{
	dictEntry* node;
	dictEntry* temp;
	// key��hashtable�ж�Ӧ���±�
	unsigned int index;
	index = d->type->hashFunction(key, d->ht->size);
	node = d->ht->table[index];
	// key��ͬ
	if (!(d->type->keyCompare(key, node->key))) {
		d->ht->table[index] = node->next;
		d->type->keyDestructor(node->key);
		d->type->valDestructor(node->value);
		free(node);
		return;
	}
	temp = node;
	node = node->next;
	while (node) {
		if (!d->type->keyCompare(key, node->key)) {
			temp->next = node->next;
			d->type->keyDestructor(node->key);
			d->type->valDestructor(node->key);
			free(node);
			return;
		}
		temp = node;
		node = node->next;
	}
	return;
}

// �ͷŸ��� �ֵ䣬�Լ��ֵ��а��������м�ֵ��
void dictRelease(dict *d)
{
	if (d == NULL) return;
	dictEntry* node;
	dictEntry* temp;
	for (int i = 0; i < d->ht->size;i++) {
		node = d->ht->table[i];
		while (node != NULL) {
		}
	}
	free(d->ht);
	free(d);
}


//int main()
//{
//	dictType *type = (dictType*)malloc(sizeof(dictType));
//	type->hashFunction = myHashFunction;
//	type->keyDup = myKeyDup;
//	type->keyDestructor = myKeyDestructor;
//	type->valDup = myValDup;
//	type->keyCompare = myKeyCompare;
//	type->valDestructor = myValDestructor;
//
//	dict* d = dictCreate(type,HASHSIZE);
//
//	const char *key1 = "sss";
//	const char *value1 = "111";
//	bool result = dictInsert(d,key1,value1);
//	if (result) {
//		printf("insert1 success\n");
//	}
//	else {
//		printf("insert1 fail\n");
//	}
//	char* temp = dictFetchValue(d, key1);
//	if (temp!="") printf(" Amd Yes!\n");
//	else printf(" Oh No!\n");
//
//
//	return 0;
//}



