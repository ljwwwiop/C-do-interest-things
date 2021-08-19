#pragma once
// 实现Redis - sds动态字符串

typedef struct sdshdr {
	// 记录buf数组中已使用字节的数量
	// 等于sds所保存字符串的长度，不包括最后的'\0'
	int len;
	// 记录buf数组中未使用字节的数量
	int free;
	char *buf;

}*sds;

// 返回sds已使用空间的字节数:len
static inline int sdslen(const sds sh)
{
	return sh->len;
}

static inline int sdsavail(const sds sh)
{
	return sh->free;
}

// 创建一个包含给定c字符串的sds
sds sdsnew(char *);

// 为sds 分配指定空间/len
sds sdsnewlen(sds, int);

// 创建一个不包含任何内容的空字符串
sds sdsempty(void);

//释放给定的sds
void sdsfree(sds);

//创建一个给定sds的副本
sds sdsdup(sds);

//清空sds保存的字符串内容
sds sdsclear(sds);

//将给定c字符串拼接到另一个sds字符串的末尾
sds sdscat(sds, char *);

//将给定sds字符串拼接到另一个sds字符串的末尾
sds sdscatsds(sds, sds);

//将给定的c字符串复制到sds里面，覆盖原有的字符串
sds sdscpy(sds, char *);

//保留sds给定区间内的数据，不在区间内的数据会被覆盖或清除
//s = sdsnew("Hello World");
//sdsrange(s,1,-1); => "ello World"
sds sdsrange(sds, int, int);

//大小写不敏感
sds sdstrim(sds, const char *);

//对比两个sds字符串是否相同
bool sdscmp(sds, sds);

