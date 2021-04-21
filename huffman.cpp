#pragma warning(disable:4996)
#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define N 256		//叶子结点数
#define M 2*N-1		//树中结点总数
#define WIDTHBYTES(bits) (((bits)+31)/32*4)///行补位
#define BYTE_NA (11054   /2)//读取文件的存入的字长度的倍数


typedef unsigned char BYTE;
typedef unsigned short WORD;


typedef struct
{
	int data;	//结点值
	int weight;		//权重
	int parent;		//双亲结点
	int lchild;		//左孩子结点
	int rchild;		//右孩子结点
} HTNode;
typedef struct
{
	unsigned char *cd;		//指向哈夫曼码的指针
	int Code_long;///该编码的长度
} HCode;

//读取bmp图片
int read_bmp(int R[],WORD UP[])
{
	FILE* in;
	if ((in = fopen("fa.bmp", "rb")) == NULL)
	{
		printf("无法打开文件\n");
		return 0;//无法打开返回0结束程序
	}
	WORD n, m;
	int i = 0;
	//利用左移和右移将字转化为双字节并统计
	while (fread(&n, sizeof(WORD), 1, in) != NULL)
	{
		m = n >> 8;
		R[m]++;
		m = n << 8;
		m = m >> 8;
		R[m]++;
		UP[i] = n;
		i++;
	}
	printf("%d", i);
	fclose(in);
	return 1;
}



void CreateHT(HTNode ht[], int n)  
{
	int i, k, lnode, rnode;
	int min1, min2;
	for (i = 0; i < 2 * n - 1; i++)			
		ht[i].parent = ht[i].lchild = ht[i].rchild = -1;
	for (i = n; i < 2 * n - 1; i++)			
	{
		min1 = min2 = 32767;			
		lnode = rnode = -1;
		for (k = 0; k <= i - 1; k++)		
			if (ht[k].parent == -1)	
			{
				if (ht[k].weight < min1)
				{
					min2 = min1;
					rnode = lnode;
					min1 = ht[k].weight;
					lnode = k;
				}
				else if (ht[k].weight < min2)
				{
					min2 = ht[k].weight; 
					rnode = k;
				}
			}
		ht[lnode].parent = i;
		ht[rnode].parent = i;	
		ht[i].weight = ht[lnode].weight + ht[rnode].weight;
		ht[i].lchild = lnode;
		ht[i].rchild = rnode;
	}
}


void DispHCode( HCode hcd[])	//输出哈夫曼编码
{
	int i, k;
	printf("结点值 及其对应的哈夫曼编码\n");
	for (i = 0; i < N; i++)
	{
		printf("%d:  ", i);
		for (k = 0; hcd[i].cd[k] != 'a'; k++)
			printf("%d", hcd[i].cd[k]);
		printf("\n");
	}
}

void CHFTCode(HTNode ht[], HCode hcd[],int pos,unsigned char R[],int n,int codem)//递归生成哈夫曼编码
{
	if (codem == -1)//树根
		;
	else if (codem == 1)//
		R[n - 1] = 1;
	else
		R[n - 1] = 0;

	///函数初始条件
	int l = ht[pos].lchild;
	int r = ht[pos].rchild;

	//递归终止条件,左右孩子均为空->
	if (ht[pos].lchild == -1 && ht[pos].rchild == -1) 
	{
		hcd[pos].cd = (BYTE*)malloc((n+1) * sizeof(BYTE));//动态分配编码的内存
		for (int i = 0; i < n; i++)
			hcd[pos].cd[i] = R[i];
		hcd[pos].Code_long = n;
		hcd[pos].cd[n] = 'a';//a为哨兵
		return;
	}
	CHFTCode(ht,hcd,l, R,n+1,0);//将函数推向左孩子
	CHFTCode(ht,hcd, r, R, n+1,1);//将函数推向右孩子

}

void HuffmanEncode(HTNode ht[], HCode hcd[],WORD UP[])
{

	//开辟缓冲区
	int BuffLength = 0;
	for (int i = 0; i < N; i++) {
		BuffLength += ht[i].weight*hcd[i].Code_long+1;
		//计算所有哈夫曼编码的总长度以确定缓冲区的大小
	}
	
	unsigned char* Buff = new unsigned char[BuffLength];

	int cur = 0,i,k;
	WORD num,n, m;

	for(k=0;k<BYTE_NA;k++)//利用左移和右移将字转化为双字节并统计,将字节转为相应哈夫曼编码
	{
		n = UP[k];
		m = n >> 8;
		for (i = 0; i < hcd[m].Code_long; i++)
		{
			Buff[cur] = hcd[m].cd[i];
			cur++;
		}
		m = n << 8;
		m = m >> 8;
		for (i = 0; i < hcd[m].Code_long; i++)
		{
			Buff[cur] = hcd[m].cd[i];
			cur++;
		}
	}

	//生成新的压缩文件
	FILE* fp;
	fp=fopen("fa.bmp.huf", "wb");			//新建文件，二进制写入
	int times = BuffLength / 16 +1;	//int——>32位,确保存入数据长度为字的倍数
	
	if (fp == NULL)
	{
		fclose(fp);
		return;
	}
	for (i = 0; i < times; i++)//压缩哈夫曼码，将8个字节表示的编码
		                        ////通过数学方法压缩至一个字节之中
	{
		n = m = 0;
		if (i == times - 1)
		{
			for (k = 0; k < 16; k++)
			{
				if (Buff[i * 16 + k] == '1')
					if(k<8)
						n +=(WORD) pow(2, 7 - double(k));
					else
						m += (WORD)pow(2, 15 - double(k));
			}
		}
		else//如果最后存储不足以凑成一个字节，则通过补零满足字节长度
		{
			for (k = 0;(i*16+k)<BuffLength; k++)
			{
				if (Buff[i * 16 + k] == '1')
					if (k < 8)
						n += (WORD)pow(2, 7 - double(k));
					else
						m += (WORD)pow(2, 15 - double(k));
			}
		}
		num = n << 8;
		num += m;
		fwrite(&num, sizeof(WORD), 1, fp); //写数据的长度
	}
	fclose(fp);
}


void exp_make(int R[],WORD UP[])//生成哈夫曼树及其编码
{
	
	HTNode ht[M];
	HCode hcd[N];
	int i,  k;
	
	for (i = 0; i < N; i++)
	{
		ht[i].data = i;
		ht[i].weight = R[i];
	}
	CreateHT(ht, N);//创建哈夫曼树
	printf("结点值   权重    双亲结点  左孩子结点 右孩子结点 ：\n");
	for (i = 0; i < M; i++)
	{
		printf("%6d %6d %6d %6d %6d\n", ht[i].data, ht[i].weight, ht[i].parent, ht[i].lchild, ht[i].rchild);
	}
	for (k = 0; k < M; k++)
	{
		if (ht[k].parent == -1)
			break;
	}
	BYTE Rn[N] = { 0 };
	CHFTCode(ht, hcd, k, Rn, 0,-1);
	
	DispHCode(hcd);

	HuffmanEncode(ht, hcd,UP);
	
	for (k = 0; k < N; k++)
	{
		free(hcd[k].cd);
	}
	free(UP);
}

int main()
{
	int R[N] = { 0 }, i;
	WORD* UP;
	UP = (WORD*)malloc(BYTE_NA * sizeof(WORD));
	i=read_bmp(R,UP);
	if (i == 0)
	{
		printf("bmp图片打开失败\n");
		return 0;
	}
	printf("256种字节及其对应个数：\n");
	for (i = 0; i < N; i++)
	{
		printf("%d，%d\n", i, R[i]);
	}

	exp_make(R,UP);
	
	return 1;
}