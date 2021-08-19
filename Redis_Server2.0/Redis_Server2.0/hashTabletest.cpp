/*
	再次复习一下hashTabel，主要顺便复习一下hash冲突中的拉链法
	dict,set底层都是哈希
	map 底层是红黑树
*/

#include <iostream>
#include <string>
#include <malloc.h>

using namespace std;

// 哈希表的大小
#define HASHSIZE 10
typedef unsigned int uint;

// 定义哈希节点
typedef struct Node {
	const char *key;
	const char *value;
	Node *next; // 节点next指针
}Node;

class HashTable {
private:
	Node* node[HASHSIZE];
public:
	// 构造
	HashTable();
	uint hash(const char *key);
	// 返回key对应的节点
	Node*lookup(const char *key);
	// 插入一个节点
	bool insert(const char *key,const char *val);
	// 获取某个key的val
	const char *get(const char *key);
	// 打印
	void display();
};

HashTable::HashTable() {
	for (int i = 0; i < HASHSIZE; i++)
		node[i] = NULL; // 初始化为NULL

}

// 定义哈希表的hash算法，除留余数法 mod = 33
uint HashTable::hash(const char *key) {
	uint hash = 0;
	// 构造一个散列表
	for (; *key; ++key)
		hash = hash * 33 + *key;
	return hash % HASHSIZE;
}

// hashTable 就是实现一个 key 查到value过程
// 如果多个值生成了同一个key的话，那么这个时候就会产生冲突
// 解决冲突就是拉链法实现，当然也可以在拓展，但是其余方法具有局限性

Node*HashTable::lookup(const char *key)
{
	Node *np;
	uint index;
	// 计算出Key
	index = hash(key);
	for (np = node[index]; np; np = np->next)
	{
		// 找到了链表中Key相同的Node
		if (!strcmp(key, np->key))
			return np;
	}
	return NULL;
}

bool HashTable::insert(const char *key, const char *value)
{
	uint index;
	Node *np;
	//这个key对应的结点（是唯一的）在表中不存在，
	//则新建一个node，插入对应的index的链表头
	//这里的链表数组的元素--结点指针一直在变
	if (!(np = lookup(key)))
	{
		index = hash(key);
		np = (Node*)malloc(sizeof(Node));
		if (!np) return false;
		np->key = key;
		np->next = node[index];
		node[index] = np;
	}
	// 如果存在那么直接修改Node值就好了
	np->value = value;
	return true;
}

void HashTable::display() {
	Node *temp;
	for (int i = 0; i < HASHSIZE; i++) {
		if (!node[i]) {
			cout << "[]" << endl;
		}
		else {
			// 存在
			cout << "[";
			for(temp = node[i];temp;temp =temp->next)
				cout << "[" << temp->key << "]" << "[" << temp->value << "]";
			cout << "]";
		}
	}
	
}

int main()
{
	HashTable *ht = new HashTable();
	const char *key[] = { "a","b" ,"c","asdhu","asd","asdsad" };
	const char *value[] = { "value1","value2" ,"zhu","asf","sfdsf","asfdsagfsg" };
	for (int i = 0; i < HASHSIZE; i++)
		ht->insert(key[i], value[i]);

	ht->display();
	
	system("pause");
	return 0;
}
