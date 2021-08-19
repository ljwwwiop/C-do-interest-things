#pragma once

// ¸´Ï°×ÖµäÊ÷
typedef struct TrieNode {
	bool isEnd;
	struct TrieNode *map[26];

}Trie;

Trie* trieCreate();

void trieInsert(Trie *obj, char *word);

bool trieSearch(Trie *obj, char *word);

// ÊÇ·ñ´æÔÚÇ°×º
bool trieStartsWith(Trie* obj, char * prefix);

// ÊÍ·Å
void trieFree(Trie *obj);





