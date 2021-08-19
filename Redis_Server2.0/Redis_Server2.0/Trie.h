#pragma once

// ��ϰ�ֵ���
typedef struct TrieNode {
	bool isEnd;
	struct TrieNode *map[26];

}Trie;

Trie* trieCreate();

void trieInsert(Trie *obj, char *word);

bool trieSearch(Trie *obj, char *word);

// �Ƿ����ǰ׺
bool trieStartsWith(Trie* obj, char * prefix);

// �ͷ�
void trieFree(Trie *obj);





