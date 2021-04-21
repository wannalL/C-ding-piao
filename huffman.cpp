#pragma warning(disable:4996)
#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define N 256		//Ҷ�ӽ����
#define M 2*N-1		//���н������
#define WIDTHBYTES(bits) (((bits)+31)/32*4)///�в�λ
#define BYTE_NA (11054   /2)//��ȡ�ļ��Ĵ�����ֳ��ȵı���


typedef unsigned char BYTE;
typedef unsigned short WORD;


typedef struct
{
	int data;	//���ֵ
	int weight;		//Ȩ��
	int parent;		//˫�׽��
	int lchild;		//���ӽ��
	int rchild;		//�Һ��ӽ��
} HTNode;
typedef struct
{
	unsigned char *cd;		//ָ����������ָ��
	int Code_long;///�ñ���ĳ���
} HCode;

//��ȡbmpͼƬ
int read_bmp(int R[],WORD UP[])
{
	FILE* in;
	if ((in = fopen("fa.bmp", "rb")) == NULL)
	{
		printf("�޷����ļ�\n");
		return 0;//�޷��򿪷���0��������
	}
	WORD n, m;
	int i = 0;
	//�������ƺ����ƽ���ת��Ϊ˫�ֽڲ�ͳ��
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


void DispHCode( HCode hcd[])	//�������������
{
	int i, k;
	printf("���ֵ �����Ӧ�Ĺ���������\n");
	for (i = 0; i < N; i++)
	{
		printf("%d:  ", i);
		for (k = 0; hcd[i].cd[k] != 'a'; k++)
			printf("%d", hcd[i].cd[k]);
		printf("\n");
	}
}

void CHFTCode(HTNode ht[], HCode hcd[],int pos,unsigned char R[],int n,int codem)//�ݹ����ɹ���������
{
	if (codem == -1)//����
		;
	else if (codem == 1)//
		R[n - 1] = 1;
	else
		R[n - 1] = 0;

	///������ʼ����
	int l = ht[pos].lchild;
	int r = ht[pos].rchild;

	//�ݹ���ֹ����,���Һ��Ӿ�Ϊ��->
	if (ht[pos].lchild == -1 && ht[pos].rchild == -1) 
	{
		hcd[pos].cd = (BYTE*)malloc((n+1) * sizeof(BYTE));//��̬���������ڴ�
		for (int i = 0; i < n; i++)
			hcd[pos].cd[i] = R[i];
		hcd[pos].Code_long = n;
		hcd[pos].cd[n] = 'a';//aΪ�ڱ�
		return;
	}
	CHFTCode(ht,hcd,l, R,n+1,0);//��������������
	CHFTCode(ht,hcd, r, R, n+1,1);//�����������Һ���

}

void HuffmanEncode(HTNode ht[], HCode hcd[],WORD UP[])
{

	//���ٻ�����
	int BuffLength = 0;
	for (int i = 0; i < N; i++) {
		BuffLength += ht[i].weight*hcd[i].Code_long+1;
		//�������й�����������ܳ�����ȷ���������Ĵ�С
	}
	
	unsigned char* Buff = new unsigned char[BuffLength];

	int cur = 0,i,k;
	WORD num,n, m;

	for(k=0;k<BYTE_NA;k++)//�������ƺ����ƽ���ת��Ϊ˫�ֽڲ�ͳ��,���ֽ�תΪ��Ӧ����������
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

	//�����µ�ѹ���ļ�
	FILE* fp;
	fp=fopen("fa.bmp.huf", "wb");			//�½��ļ���������д��
	int times = BuffLength / 16 +1;	//int����>32λ,ȷ���������ݳ���Ϊ�ֵı���
	
	if (fp == NULL)
	{
		fclose(fp);
		return;
	}
	for (i = 0; i < times; i++)//ѹ���������룬��8���ֽڱ�ʾ�ı���
		                        ////ͨ����ѧ����ѹ����һ���ֽ�֮��
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
		else//������洢�����Դճ�һ���ֽڣ���ͨ�����������ֽڳ���
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
		fwrite(&num, sizeof(WORD), 1, fp); //д���ݵĳ���
	}
	fclose(fp);
}


void exp_make(int R[],WORD UP[])//���ɹ��������������
{
	
	HTNode ht[M];
	HCode hcd[N];
	int i,  k;
	
	for (i = 0; i < N; i++)
	{
		ht[i].data = i;
		ht[i].weight = R[i];
	}
	CreateHT(ht, N);//������������
	printf("���ֵ   Ȩ��    ˫�׽��  ���ӽ�� �Һ��ӽ�� ��\n");
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
		printf("bmpͼƬ��ʧ��\n");
		return 0;
	}
	printf("256���ֽڼ����Ӧ������\n");
	for (i = 0; i < N; i++)
	{
		printf("%d��%d\n", i, R[i]);
	}

	exp_make(R,UP);
	
	return 1;
}