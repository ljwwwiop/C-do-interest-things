/*
	��Ҫʵ��ʹ���ֵ���Trie��ʵ��һ�����ʹ��˼�����
	�ؼ��ֹ����㷨
*/

#include <cstdint>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>
#include <string.h>

using namespace std;

// Trie���㷨

class WordFilter {
public:
	WordFilter(){}
	~WordFilter()
	{
		// �ͷſռ�
		clean(&m_Filter);
	}

	bool Load(const char* filename)
	{
		// I/O
		ifstream fin(filename);
		if (!fin.is_open())
		{
			return false;
		}

		// ������
		string inbuf;
		while (!fin.eof())
		{
			// ��ȡһ��
			getline(fin, inbuf, '\n');

			if (inbuf.size() == 0) continue;

			// ���÷���
			AddWord(inbuf.c_str());
		}
		return true;
	}

	void AddWord(const char* word)
	{
		int len = strlen(word);
		Filter* filter = &m_Filter;

		for (int i = 0; i < len; i++)
		{
			unsigned char c = word[i];
			if (i == len - 1)
			{
				filter->m_NodeArray[c].m_Flag |= FilterNode::NODE_IS_END;
				break;
			}
			else
			{
				filter->m_NodeArray[c].m_Flag |= FilterNode::NODE_HAS_NEXT;
			}

			if (filter->m_NodeArray[c].m_NextFilter == NULL)
			{
				Filter* tmpFilter = new Filter();
				filter->m_NodeArray[c].m_NextFilter = tmpFilter;
			}

			filter = (Filter *)filter->m_NodeArray[c].m_NextFilter;
		}
	}

	void AddWords(const set<string>& wordList)
	{
		// һ����
		for (set<string>::const_iterator it = wordList.begin(); it != wordList.end(); it++)
		{
			AddWord(it->c_str());
		}
	}
	// ������������
	void AddWords(const vector<string>& wordList)
	{
		for (auto it = wordList.begin(); it != wordList.end(); it++)
		{
			AddWord(it->c_str());
		}
	}

	int check(const char* str)
	{
		Filter* filter = NULL;
		for (int i = 0;i<(int)strlen(str) - 1;++i)
		{
			filter = &m_Filter;
			for (int j = i; j < strlen(str); j++)
			{
				unsigned char c = str[j];
				if (filter->m_NodeArray[c].m_Flag == FilterNode::NODE_IS_NULL)
				{
					break;
				}
				else if (filter->m_NodeArray[c].m_Flag == FilterNode::NODE_IS_END)
					return i;
				else
					filter = (Filter *)filter->m_NodeArray[c].m_NextFilter;
			}
		}
		return -1;
	}

	void CheckAndModify(char *str,const char replace = '*')
	{
		// �������޸ĵ�����ַ���
		Filter * filter = NULL;
		for (int32_t i = 0; i < (int)strlen(str)-1; ++i)
		{
			filter = &m_Filter;
			for (uint32_t j = i; j < strlen(str); ++j)
			{
				unsigned char c = str[j];

				if (filter->m_NodeArray[c].m_Flag == FilterNode::NODE_IS_NULL) break;
				else if (filter->m_NodeArray[c].m_Flag == FilterNode::NODE_IS_END)
				{
					for (uint32_t k = i; k <= j; ++k) {
						// �滻��
						str[k] = replace;
					}
					if (filter->m_NodeArray[c].m_Flag & FilterNode::NODE_HAS_NEXT)
					{
						filter = (Filter *)filter->m_NodeArray[c].m_NextFilter;
					}
					else {
						continue;
					}
				}
				else {
					// next ��ֱ�Ӽ���next
					filter = (Filter*)filter->m_NodeArray[c].m_NextFilter;
				}
			}
		}

	}
	// ������������
	void CheckAndModify(std::string& str, const char replace = '*')
	{
		Filter* filter = NULL;
		for (int32_t i = 0; i < (int)str.size() - 1; i++)
		{
			filter = &m_Filter;
			for (uint32_t j = i; j < str.size(); j++)
			{
				unsigned char c = str[j];
				//if ((c >= 'A' && c <= 'Z'))
				//{
				//	c += 32;
				//}
				if (filter->m_NodeArray[c].m_Flag == FilterNode::NODE_IS_NULL)
				{
					break;
				}
				else if (filter->m_NodeArray[c].m_Flag & FilterNode::NODE_IS_END)
				{
					for (uint32_t k = i; k <= j; k++)
					{
						str[k] = replace;
					}

					if (filter->m_NodeArray[c].m_Flag & FilterNode::NODE_HAS_NEXT)
					{
						filter = (Filter*)filter->m_NodeArray[c].m_NextFilter;
					}
					else
					{
						continue;
					}
				}
				else // NODE_HAS_NEXT
				{
					filter = (Filter*)filter->m_NodeArray[c].m_NextFilter;
				}
			}
		}
	}

private:
	struct FilterNode {
		char m_Flag;
		void* m_NextFilter;
		enum Flag
		{
			NODE_IS_NULL = 0x00,
			NODE_HAS_NEXT = 0x01,
			NODE_IS_END = 0x10,
		};
		FilterNode() :m_Flag(NODE_IS_NULL), m_NextFilter(NULL) {}
	};

	struct Filter
	{
		FilterNode m_NodeArray[256];
	}m_Filter;

	void clean(Filter* filter) {
		for (uint32_t i = 0; i < 256; i++)
		{
			if (filter->m_NodeArray[i].m_NextFilter)
			{
				clean((Filter *)filter->m_NodeArray[i].m_NextFilter);
				delete (Filter*)filter->m_NodeArray[i].m_NextFilter;;
			}
		}
	}
};

int main()
{
	WordFilter wf;
	wf.Load("dirtyword.key");
	
	// �����Ҫ���˵�string
	wf.AddWord("abc");
	wf.AddWord("����");
	wf.AddWord("����δ");

	string s = "aaagong����δchandang nihao abc a ���� ½���¼� ����fuck ";
	cout << s << wf.check(s.c_str()) << endl;
	wf.CheckAndModify(s);
	cout << "�޸ĺ������ :" << s << endl;

	ifstream fin("dirtyword.key");
	if (!fin.is_open())
	{
		return -1;
	}

	string inbuf;
	while (!fin.eof())
	{
		// ��ȡһ��
		getline(fin, inbuf, '\n');
		if (inbuf.size() == 0)
		{
			continue;
		}

		cout <<"inbuf :"<<inbuf << endl;
		inbuf += char(rand() % 36 + 97);
		wf.CheckAndModify(inbuf);

		cout << inbuf << endl;

	}


	return 0;
}
