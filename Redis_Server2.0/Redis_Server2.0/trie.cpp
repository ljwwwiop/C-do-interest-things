#include "Trie.h"
#include <iostream>
using namespace std;

Trie* trieCreate() {
	Trie *obj = (Trie*)malloc(sizeof(Trie));
	obj->isEnd = false;
	for (int i = 0; i < 26; i++)
		obj->map[i] = NULL;
	return obj;
}

void trieInsert(Trie *obj, char *word) 
{
	while (*word)
	{
		if (obj->map[*word - 'a'] == NULL) // ��û�д���
			obj->map[*word - 'a'] = trieCreate();
		// �������ֱ����һ����ĸ
		obj = obj->map[*word - 'a'];
		word++;
	}
	obj->isEnd = true;
}

bool trieSearch(Trie *obj, char *word)
{
	while (*word)
	{
		if (obj->map[*word - 'a'] == NULL) return false;
		obj = obj->map[*word - 'a'];
		word++;
	}
	return true;
}


bool trieStartsWith(Trie* obj, char * prefix)
{
	while (*prefix)
	{
		if (obj->map[*prefix - 'a'] == NULL) return false;
		obj = obj->map[*prefix - 'a'];
		prefix++;
	}
	return true;
}

// �ͷ�
void trieFree(Trie *obj)
{
	for (int i = 0; i < 26; i++)
		if (obj->map[i] != NULL)
			trieFree(obj->map[i]);
	free(obj);
}



//int main()
//{
//	Trie* obj = trieCreate();
//	char *word = "pop";
//	trieInsert(obj,word);
//	char *s1 = "po";
//	char *s2 = "lol";
//
//	bool flag = trieStartsWith(obj,s1);
//	cout << flag << endl;
//	flag = trieStartsWith(obj, s2);
//	cout << flag << endl;
//
//	return 0;
//}



