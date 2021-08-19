#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

using namespace std;
// 定义哈希表的大小
#define HASHSIZE 10
/*
// 定义哈希表进行相关操作的函数
// 计算哈希值的函数
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

// 复制键的函数
char *myKeyDup(char *key) {
	return key;
}
//复制值的函数
char *myValDup(char *obj) {
	return obj;
}
//对比键的函数
int myKeyCompare(const char *key1, char *key2) {
	char*charkey1 = (char*)key1;
	char*charkey2 = (char*)key2;
	return strcmp(charkey1, charkey2);
}
//销毁键的函数
void myKeyDestructor(char *key) {
	free(key);
}
//销毁值的函数
void myValDestructor(char *obj) {
	free(obj);
}*/

// 创建一个新的字典
dict *dictCreate(dictType *type,int hashSize)
{
	// 申请内存大小
	dict* d = (dict*)malloc(sizeof(dict));
	if (d == NULL) {
		printf("Lian Redis Error : Dict malloc fail.\n");
		return NULL;
	}
	// 对hashtable进行相关操作的特定函数集
	if(type == NULL) printf("Lian Redis Warning : Type is null.\n");
	d->type = type;
	// 初始化哈希表
	d->ht = (dictht*)malloc(sizeof(dictht));
	d->ht->size = hashSize;
	d->ht->used = 0;
	d->ht->table = (dictEntry**)malloc(sizeof(dictEntry*)*hashSize);
	// 全部节点都设为null
	for (int i = 0; i < hashSize; i++)
		d->ht->table[i] = NULL;
	return d;

}

// 根据key寻找其在hashTable中对应的节点
dictEntry* lookup(dict *d,char* key)
{
	dictEntry* node;
	// 该key在hashtable中对应的下标
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

// 将给定的键值对 - 添加到字典里面
bool dictInsert(dict *d, char* key,char* val)
{
	unsigned int index;
	dictEntry* node;
	//不存在-进行插入操作
	if (!(node = lookup(d, key))) {
		index = d->type->hashFunction(key, d->ht->size);
		node = (dictEntry*)malloc(sizeof(dictEntry));
		if (node == NULL) return false;

		//TODO
		node->key = (char*)malloc(sizeof(char)*(strlen(key) + 1));
		strcpy(node->key, key);
		// 链上
		node->next = d->ht->table[index];
		// 赋值
		d->ht->table[index] = node;
	}
	//若存在——进行更新操作,直接修改其对应的value值
	//TODO
	node->value = (char*)malloc(sizeof(char)*(strlen(val) + 1));
	strcpy(node->value, val);

	return true;
}

// 返回给定的key的value
char* dictFetchValue(dict *d,char* key) {
	dictEntry* node;
	//找不到这个结点
	if (!(node = lookup(d, key))) {
		return "";
	}
	//cout<<node->value<<"coutcoutcout"<<endl;
	//printf("printf : %s\n",node->value);
	return node->value;
}


// 从字典中删除给定key对应的value
void dictDelete(dict *d, char* key)
{
	dictEntry* node;
	dictEntry* temp;
	// key在hashtable中对应的下标
	unsigned int index;
	index = d->type->hashFunction(key, d->ht->size);
	node = d->ht->table[index];
	// key相同
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

// 释放给定 字典，以及字典中包含的所有键值对
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



