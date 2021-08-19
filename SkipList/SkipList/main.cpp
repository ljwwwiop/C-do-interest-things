#include <iostream>
#include <vector>

using namespace std;

/*
	Q��Ϊʲôredis�ײ�Ҫʹ��������ʵ�֣�
	 * ����ʵ��
	 * ���ֲ�
	 * ֱ��Ѱ����һ��
	 * ÿ3��Ϊһ��
	 * Ҫ֪����ô��
	 * �������ϵ����ݽṹ
*/

class SingleLinkList {
public:
	int val;
	SingleLinkList *next;
	SingleLinkList *down; // ��һ����Ծ��
	SingleLinkList() = default;

private:
	// �������캯��
	SingleLinkList(const SingleLinkList&);
	SingleLinkList& operator=(const SingleLinkList&);

};

// β�巨��������
SingleLinkList* insertRail(SingleLinkList *head,int val) 
{
	auto tmp = head;
	while (tmp->next) {
		tmp = tmp->next;
	}
	SingleLinkList *newRail = new SingleLinkList();
	tmp->next = newRail;
	newRail->val = val;
	return head;
}

// ����ʵ�� ����������ÿ3��������һ������
SingleLinkList* produceLevelNode(SingleLinkList *head,int len) 
{
	if (len / 2 <= 1) return head;
	vector<SingleLinkList *> vecIndex; // vector �д洢ָ�� index
	int numOfIndex = len >> 1; // ����һλ ���� /2
	while (head->next && numOfIndex)
	{
		SingleLinkList *index = new SingleLinkList();
		index->val = head->val;
		index->down = head;
		vecIndex.push_back(index);
		head = head->next;
		--numOfIndex;
	}
	for (int i = 1; i < len / 2; i++) {
		vecIndex[i - 1]->next = vecIndex[i]; // ��������������
	}
	head = vecIndex[0];
	return head;
}

// ��������
bool searchElemSkip(SingleLinkList *head,int elem)
{
	if (head->val == elem) return true;
	if (elem < head->val) return false; // ������ܴ���bug
	
	while (head) {
		if (head->next) {
			if (elem >= head->val && elem <= head->next->val) {
				// ������
				if (head->down) { head = head->down; }
				else {
					while (head) {
						if (head->val == elem) return true;
						head = head->next;
					}
				}
			}
			else if (elem>head->next->val && head->next)
			{
				// ����һ�� �� ��һ���м�
				head = head->next;
			}
			else {
				head = head->down;
			}
		}
		else {
			head = head->down;
		}

	}
	return false;
}

// ���� ��Ҫ�������ɲ������߲����в����µĽڵ�

int main()
{
	SingleLinkList *skipNode = new SingleLinkList();
	int *arr = new int[10]{ 1,2,3,4,4,5,7,8,9,9 };
	
	for (int i = 0; i < 10; i++) skipNode = insertRail(skipNode, arr[i]);

	// ���ɶ��skip level
	int len = 10;
	while (len) {
		skipNode = produceLevelNode(skipNode, len);
		len = len >> 1;
	}

	if (searchElemSkip(skipNode, 11)) {
		cout << "find :"<< searchElemSkip(skipNode, 11) << endl;
	}
	else {
		cout << "no no no" << endl;
	}

	return 0;
}




