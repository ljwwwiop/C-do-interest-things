#include "sds.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
// ����һ����������c�ַ�����sds
/*
	redis��û��ʹ��C��ͳ���ַ���
	��ΪC�ַ�����������redis���ַ����İ�ȫ�ԡ�Ч���Լ����ܷ��������
	SDS ��̬�ַ����ṹ �������ݣ���ǰ����len��ʣ��ռ�free��buf�洢����
*/
sds sdsnew(const char *init)
{
	sds sh = (sds)malloc(sizeof(struct sdshdr));
	sh->len = strlen(init);
	sh->free = 0;

	sh->buf = (char*)malloc(sizeof(char)*(sh->len + 1));
	// ���ַ������ݽ��и���
	int i;
	for (i = 0; i < sh->len; i++) {
		(sh->buf)[i] = init[i];
	}
	(sh->buf)[i] = '\0';
	return sh;
}

// Ϊsds����ָ���ռ�/len
sds sdsnewlen(sds sh,int len)
{
	int i;
	sh->free = len - 1 - sh->len;
	//����֮ǰ��buf����
	char *str = (char *)malloc(sizeof(char)*(sh->len + 1));
	for (i = 0; i < (sh->len); i++) {
		str[i] = sh->buf[i];
	}
	str[i] = '\0';
	//sh->buf=(char*)realloc(sh->buf,len);
	sh->buf = (char*)malloc(sizeof(char)*len);
	for (i = 0; i < (sh->len); i++) {
		sh->buf[i] = str[i];
	}
	sh->buf[i] = '\0';
	free(str);
	return sh;
}

// ����һ���������κ����ݵĿ��ַ���
sds sdsempty(void)
{
	sds sh = (sds)malloc(sizeof(struct sdshdr));
	sh->len = 0;
	sh->free = 0;
	sh->buf = (char*)malloc(sizeof(char));
	sh->buf[0] = '\0'; // �ַ���������
	return sh;
}

// �ͷŸ�����sds
void sdsfree(sds *sh)
{
	(*sh)->free = 0;
	(*sh)->len = 0;
	free((*sh)->buf);
	free(*sh);
}

// ����һ��������sds�ĸ���
sds sdsdup(sds sh01)
{
	sds sh02 = (sds)malloc(sizeof(struct sdshdr));
	sh02->free = sh01->free;
	sh02->len = sh02->len;
	sh02->buf = (char*)malloc(sizeof(char)*(sh02->free + sh02->len + 1));
	int i;
	for (i = 0; i < sh01->len; i++)
	{
		sh02->buf[i] = sh01->buf[i];
	}
	sh02->buf[i] = '\0';
	return sh02;

}

// ���sds������ַ�������
sds sdsclear(sds sh)
{
	int total = sh->len + sh->free + 1;
	sh->len = 0;
	sh->free = total - 1;
	sh->buf[0] = '\0';
	return sh;
}

//������c�ַ���ƴ�ӵ���һ��sds�ַ�����ĩβ
//�ȼ��sds�Ŀռ��Ƿ������޸������Ҫ����
//�����������Զ���sds�ռ���չ��ִ���޸�����
//Ҫ�Ĵ�С��Ȼ����ִ��ʵ�ʵ��޸Ĳ���������ֹ
//���������
//��չ�ռ��ԭ��ƴ�Ӻ���ַ�����n���ֽڣ���
//�ٸ������n���ֽڵ�δʹ�ÿռ䣬buf�����ʵ�ʳ���Ϊn+n+1
//��n����1MB��ʱ����Ϊ�����1MB��δʹ�ÿռ�
//�����ַ���cat���м�ʹ�ÿո����
sds sdscat(sds sh, const char *str) {
	int newlen = strlen(str);
	int newfree;
	//ʣ��Ŀռ䲻��cat����
	if (sh->free <= newlen) {
		//�������ֵĿռ�
		newfree = newlen - sh->free;
		if (newfree < 1024) {
			newfree = newfree + newfree + 1 + sh->len + sh->free;
			sh = sdsnewlen(sh, newfree);
		}
		else {
			newfree = newfree + 1024 + 1 + sh->len + sh->free;
			sh = sdsnewlen(sh, newfree);
		}
	}
	int i;
	//ִ��cat����,�м������һ�� ' '�ո�
	sh->buf[sh->len] = ' ';
	for (i = 0; i < newlen; i++) {
		sh->buf[sh->len + i + 1] = str[i];
	}
	sh->buf[sh->len + i + 1] = '\0';
	sh->len += (newlen + 1);
	sh->free -= newlen;
	return sh;
}

// ������sds�ַ������ֵ���һ��sds�ַ�����ĩβ
sds sdscatsds(sds sh,sds str)
{
	int newlen = str->len;
	int newfree;
	// ʣ��Ŀռ䲻��cat
	if (sh->free <= newlen) {
		// �����Ĳ���
		newfree = newlen - sh->free;
		if (newfree < 1024) {
			newfree = newfree + newfree + 1 + sh->len + sh->free;
			sh = sdsnewlen(sh, newfree);
		}
		else {
			newfree = newfree + 1024 + 1 + sh->len + sh->free;
			sh = sdsnewlen(sh, newfree);

		}
	}
	int i;
	// ִ��cat
	sh->buf[sh->len] = ' ';
	for (i = 0; i < newlen; i++) {
		sh->buf[sh->len + i + 1] = str->buf[i];
	}
	// ��ֵ������
	sh->buf[sh->len + i + 1] = '\0';
	sh->len += (newlen + 1);
	sh->free = newlen;
	return sh;
}

//��������c�ַ������Ƶ�sds���棬����ԭ�е��ַ���
//��Ҫ�ȼ��
sds sdscpy(sds sh, char *str) {
	//�����ĳ���
	int len = strlen(str);
	//��Ҫʹ�õ����¿ռ䳤��
	int newlen = len - sh->len;
	int total;
	//ʣ��Ŀռ䲻������Ҫ���·��䣬��copy
	if (newlen >= sh->free) {
		//�¿ռ䳤�ȴ���1M,��ֻ�����newlen+1M+1
		//�ܵĿռ���len+newlen+1M+1
		if (newlen >= 1024) {
			total = len + newlen + 1024 + 1;
			//copy��ʹ�õ���len,�������ַ����ĳ���
			sh->len = len;
			//���еĿռ䳤��
			//sh->free=total-len-1;
			//sh->buf=(char*)realloc(sh->buf,total);
			sh = sdsnewlen(sh, total);
			//����newlen+newlen+1
		}
		else {
			total = len + newlen + newlen + 1;
			sh->len = len;
			//sh->free=total-len-1;
			//sh->buf=(char*)realloc(sh->buf,total);
			sh = sdsnewlen(sh, total);
		}
		if (sh->buf == NULL) {
			printf("PIG Redis ERROR : Realloc failed.\n");
		}
	}
	else {
		//ʣ��Ŀռ乻,����Ҫ����
		//ԭ��ӵ�е��ܿռ�
		total = sh->len + sh->free;
		sh->len = len;
		sh->free = total - sh->len;
	}
	//��ʼcopy
	int i;
	for (i = 0; i < len; i++) {
		(sh->buf)[i] = str[i];
	}
	sh->buf[i] = '\0';
	return sh;
}

//����sds���������ڵ����ݣ����������ڵ����ݻᱻ���ǻ����
//s = sdsnew("Hello World");
//sdsrange(s,1,-1); => "ello World"
sds sdsrange(sds sh, int start, int end) {
	int newlen = end - start + 1;
	char *str = (char*)malloc(sizeof(char)*(sh->len + 1));
	//sh1->free=sh->len-sh1->len;
	int i, j;
	for (i = start, j = 0; i <= end; i++, j++) {
		str[j] = sh->buf[i];
	}
	str[j] = '\0';
	sh->buf = (char*)malloc(sizeof(char)*(sh->len + 1));
	sh->free = sh->len - newlen;
	sh->len = newlen;
	for (i = 0; i < strlen(str); i++) {
		sh->buf[i] = str[i];
	}
	sh->buf[i] = '\0';
	free(str);
	return sh;
}

//����һ��sds��һ��c�ַ�����Ϊ��������sds���Ƴ�������c�ַ����г��ֹ����ַ�
//s = sdsnew("AA...AA.a.aa.aHelloWorld     :::");
//s = sdstrim(s,"A. :");
//printf("%s\n", s);
//Output will be just "Hello World".
//�ضϲ�����Ҫͨ���ڴ��ط������ͷ��ַ����в���ʹ�õĿռ䣬���������ڴ�й©
//��Сд������
//ʹ�ö��Կռ��ͷ��Ż��ַ��������̲���,ִ�����̲�����ʱ�򣬲�����ʹ���ڴ��ط�
//�����������̺��������ֽڣ�����ʹ��free���Լ�¼��Щ�ֽڣ��ȴ�����ʹ��
sds sdstrim(sds s, const char *chstr);

//�Ա�����sds�ַ����Ƿ���ͬ
bool sdscmp(sds sh1, sds sh2) {
	if (sh1->len != sh2->len) {
		return false;
	}
	for (int i = 0; i < sh1->len; i++) {
		if (sh1->buf[i] != sh2->buf[i]) {
			return false;
		}
	}
	return true;
}


/*int main()
{
	printf("sdsnew('sss')\n");
	sds sh = sdsnew("sspptt");
	printf("%s\n",sh->buf);
	printf("%d\n", sh->len);
	printf("%d\n", sh->free);

	printf("sdscat(sh,'www')\n");
	sh = sdscat(sh, "www");
	printf("%s\n", sh->buf);
	for (int i = 0; i < sh->len; i++) {
		printf("%c", sh->buf[i]);
	}
	printf("%d\n", sh->len);
	printf("%d\n", sh->free);

	sds sh2 = sdsnew("qqqq");
	sh = sdscatsds(sh, sh2);
	printf("%s\n",sh->buf);

	sdsfree(&sh);
	sdsfree(&sh2);
	system("pause");
	return 0;
}
*/

