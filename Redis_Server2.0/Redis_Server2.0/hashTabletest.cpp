/*
	�ٴθ�ϰһ��hashTabel����Ҫ˳�㸴ϰһ��hash��ͻ�е�������
	dict,set�ײ㶼�ǹ�ϣ
	map �ײ��Ǻ����
*/

#include <iostream>
#include <string>
#include <malloc.h>

using namespace std;

// ��ϣ��Ĵ�С
#define HASHSIZE 10
typedef unsigned int uint;

// �����ϣ�ڵ�
typedef struct Node {
	const char *key;
	const char *value;
	Node *next; // �ڵ�nextָ��
}Node;

class HashTable {
private:
	Node* node[HASHSIZE];
public:
	// ����
	HashTable();
	uint hash(const char *key);
	// ����key��Ӧ�Ľڵ�
	Node*lookup(const char *key);
	// ����һ���ڵ�
	bool insert(const char *key,const char *val);
	// ��ȡĳ��key��val
	const char *get(const char *key);
	// ��ӡ
	void display();
};

HashTable::HashTable() {
	for (int i = 0; i < HASHSIZE; i++)
		node[i] = NULL; // ��ʼ��ΪNULL

}

// �����ϣ���hash�㷨������������ mod = 33
uint HashTable::hash(const char *key) {
	uint hash = 0;
	// ����һ��ɢ�б�
	for (; *key; ++key)
		hash = hash * 33 + *key;
	return hash % HASHSIZE;
}

// hashTable ����ʵ��һ�� key �鵽value����
// ������ֵ������ͬһ��key�Ļ�����ô���ʱ��ͻ������ͻ
// �����ͻ����������ʵ�֣���ȻҲ��������չ���������෽�����о�����

Node*HashTable::lookup(const char *key)
{
	Node *np;
	uint index;
	// �����Key
	index = hash(key);
	for (np = node[index]; np; np = np->next)
	{
		// �ҵ���������Key��ͬ��Node
		if (!strcmp(key, np->key))
			return np;
	}
	return NULL;
}

bool HashTable::insert(const char *key, const char *value)
{
	uint index;
	Node *np;
	//���key��Ӧ�Ľ�㣨��Ψһ�ģ��ڱ��в����ڣ�
	//���½�һ��node�������Ӧ��index������ͷ
	//��������������Ԫ��--���ָ��һֱ�ڱ�
	if (!(np = lookup(key)))
	{
		index = hash(key);
		np = (Node*)malloc(sizeof(Node));
		if (!np) return false;
		np->key = key;
		np->next = node[index];
		node[index] = np;
	}
	// ���������ôֱ���޸�Nodeֵ�ͺ���
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
			// ����
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
