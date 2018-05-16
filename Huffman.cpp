
#include"Huffman.h"

#include<cstring>
using std::memset;
using std::strcmp;
using std::strlen;

#include<stack>
using std::stack;

#include<algorithm>
using std::sort;

#include<cstdio>
using std::FILE;

#include<iostream> 
using std::cout;
using std::cin;
using std::endl;

#include<iomanip>
using std::setw;
using std::right;
using std::left;

//�����޷���int �����ֵ
//f =15, ת��Ϊ�������� 1111
#define MAX 0xffffffff

/*****************************************************************
������HuffmanFileCode(��
���ܣ� ΪHuffmanFileCode��Ĺ��캯������ʼ����Ա������
*****************************************************************/
HuffmanFileCode::HuffmanFileCode()
{
	kind = 0;
	fileSize = 0;
	freqTable = NULL;
	huffTree = NULL;
	codeTable = NULL;
	memset(iFileName, 0, sizeof(iFileName));
	memset(oFileName, 0, sizeof(oFileName));
}

/*****************************************************************
����(�������� void getFilesName()
���ܣ� �õ��Ӽ�������������ļ�������ļ���·�������ƣ�
*****************************************************************/
inline void HuffmanFileCode::getFilesName()
{
	cout << "\n������Ҫ�������ļ�·�������ƣ�";
	cin >> iFileName;

	cout << "\n������Ҫ������ļ���·�������ƣ�";
	cin >> oFileName;

	cout << endl;
}

//����ȽϺ���������freqTable[]���ַ�Ƶ��������
bool cmpFreq(sCharFreq x, sCharFreq y)
{
	return x.freq > y.freq;
}

/*****************************************************************
������bool countCharFreq()
���ܣ�����ַ�����kind, �ļ���СfileSize, �Լ�Ƶ����freqTableC[];
����ֵ��true���ɹ���ȡ�� false����ȡʧ��
*****************************************************************/
bool HuffmanFileCode::countCharFreq()
{
	//�Զ����Ƶķ�ʽ���ļ������հ��ȡ
	//���á�r", ���������С�\t\n"ʱ�����Զ�����Ϊ"\n"��ȡ
	FILE *readFile = fopen(iFileName, "rb");

	//���ļ�������ʱ
	if (NULL == readFile) {
		return false;
	}

	freqTable = new sCharFreq[256];

	//��ʼ��Ƶ����
	for (int i = 0; i < 256; ++i)
	{
		freqTable[i].uch = (unsigned char)i;
		freqTable[i].freq = 0;
	}

	//ÿ��ʹ��ʱ��ʼ��
	kind = 0;
	fileSize = 0;

	fseek(readFile, 0L, SEEK_END);   //���ļ�ָ�����õ��ļ�β
	fileSize = ftell(readFile);            //��ȡ�ļ���С
	fseek(readFile, 0L, SEEK_SET);   //���ļ�ָ�����¶���ĵ��ļ���ͷ

	unsigned char temp;

	/*
	   ����feof(),�ڶ����ļ������һ���ַ���readFile->flag��Ȼû�б���Ϊ_IOEOF�����feof()��Ȼû��̽�⵽�ļ���β��
	   ֱ���ٴε���fread()ִ�ж�������feof()����̽�⵽�ļ���β�������Ͷ�ִ����һ�Ρ�����Ӧ���ȶ�ȡ���жϡ�
	*/
	fread(&temp, sizeof(unsigned char), 1, readFile);
	while (!feof(readFile))
	{
		//ͳ���ַ�temp���ֵ�Ƶ��
		++freqTable[temp].freq;

		//���ļ��ж�ȡһ���ֽڵ�temp�У�unsigned char ռһ���ֽڣ�
		fread(&temp, sizeof(unsigned char), 1, readFile);
	}

	//����freqTable[]���ַ�Ƶ��������
	sort(freqTable, freqTable + 256, cmpFreq);

	int i;
	//����ַ�������
	for (i = 0; freqTable[i].freq > 0; ++i);

	kind = i;

	fclose(readFile);

	return true;
}

/*****************************************************************
������void showCharFreq()
���ܣ���ӡfreqTableC[]�������
*****************************************************************/
void HuffmanFileCode::showFreqTable()
{
	cout << "\nͳ�Ƶõ����ַ�Ƶ�������£�\n" << endl;
	cout << right << setw(6) << "�ַ�" << '\t' << left << setw(32) << "Ƶ��\n" << endl;

	for (int i = 0; i < kind; ++i)
	{
		cout << right << setw(6) << freqTable[i].uch << '\t' << left << setw(32) << freqTable[i].freq << endl;
	}

	cout << "\n�ַ�Ƶ�����ӡ��ϡ�\n" << endl;
}

/*****************************************************************
������void selectMins(huffNode *pt, int &lc, int &rc)
������ָ������������ptָ�룬int�͵Ĳ��ҳ���len,
		  ����int�͵����Һ�����ŵ����ã�
���ܣ�ѡ��ptָ���huffNode[]����С��������㣬����������ţ�
*****************************************************************/
void HuffmanFileCode::selectMins(huffNode *pt, int len, int *lc, int *rc)
{
	unsigned int min = MAX;
	int pos = 0;
	int i;

	//ɸѡ��freqֵ��С�Ľ�㣬����Ÿ���lc
	for (i = 0; i < len; ++i)
	{
		//��pt[i].parent == 0 ��ʾ�ý�㻹δ��������
		if (pt[i].freq < min && -1 == pt[i].father)
		{
			min = pt[i].freq;
			pos = i;
		}
	}
	*lc = pos;

	min = MAX;
	for (i = 0; i < len; i++)
	{
		if (pt[i].freq < min && -1 == pt[i].father && i != *lc)
		{
			min = pt[i].freq;
			pos = i;
		}
	}
	*rc = pos;
}

/*****************************************************************
������void creatHuffTree()
���ܣ������ַ�Ƶ����freqTable[]����������kind,
		   ��kind>1ʱ�� ������������huffTree[];
*****************************************************************/
void HuffmanFileCode::creatHuffTree()
{
	//����������У�����n���ⲿ����ǣ�����n-1���ڲ����
	//�ַ�������kind��Ϊ�ⲿ���ĸ���
	huffTree = new huffNode[2 * kind - 1];

	int i;
	//��ʼ���ⲿ���(���ַ�Ƶ���ɴ�С��
	for (i = 0; i < kind; ++i)
	{
		huffTree[i].uch = freqTable[i].uch;
		huffTree[i].freq = freqTable[i].freq;
		huffTree[i].lchild = huffTree[i].rchild = -1;
		huffTree[i].father = -1;
	}

	int lc, rc;
	//�����ڲ���㣬������������
	for (; i < 2 * kind - 1; ++i)
	{
		selectMins(huffTree, i, &lc, &rc);

		huffTree[i].lchild = lc;
		huffTree[i].rchild = rc;
		huffTree[i].uch = '\0';
		huffTree[i].freq = huffTree[lc].freq + huffTree[rc].freq;
		huffTree[i].father = -1;

		huffTree[lc].father = huffTree[rc].father = i;
	}
}

/*****************************************************************
������void  coding()
���ܣ��������ɵ�huffTree, ���ļ����б��룬�õ������codeTable
*****************************************************************/
void HuffmanFileCode::coding()
{
	codeTable = new huffCode[kind];

	int fa;
	int son;
	int codeSize;
	stack<char>codeStack;   //����ջ, ���ڴ洢�ַ�����

	for (int i = 0; i < kind; ++i)
	{
		son = i;
		fa = huffTree[son].father;

		//���ⲿ��㿪ʼ�����������ݣ����������ջ��
		//��fa == -1 ,���ʾ��������
		while (fa != -1)
		{
			if (son == huffTree[fa].lchild)
				codeStack.push('0');
			else
				codeStack.push('1');

			son = fa;
			fa = huffTree[son].father;
		}

		codeSize = codeStack.size();
		codeTable[i].code = new char[codeSize + 1];

		//��ջ�еı�����Ϣ����������
		codeTable[i].uch = huffTree[i].uch;

		int j;
		for (j = 0; j < codeSize; ++j)
		{
			codeTable[i].code[j] = codeStack.top();
			codeStack.pop();
		}
		codeTable[i].code[j] = '\0';
	}
}

/*****************************************************************
void  showCodeTable()
���ܣ���ӡ������е���Ϣ
*****************************************************************/
void HuffmanFileCode::showCodeTable()
{
	if (kind > 1)
	{
		cout << "\nͳ�Ƶõ��ı�������£�\n" << endl;
		cout << right << setw(6) << "�ַ�" << '\t' << "����\n" << endl;

		for (int i = 0; i < kind; ++i)
		{
			cout << right << setw(6) << codeTable[i].uch << '\t' << codeTable[i].code << endl;
		}

		cout << "\n������ӡ��ϡ�\n" << endl;
	}
	else
		cout << "\n�ļ��н���һ���ַ��������Ϊ�գ��޷���ʾ��\n" << endl;
}

/*****************************************************************
������void  saveCode()
���ܣ����ݱ�����ļ��ı�����Ϣ�Զ�������ʽ�洢��ָ�����ļ���
*****************************************************************/
void HuffmanFileCode::saveCodes()
{
	//�Զ�������ʽ��Ҫ������ļ����������ڣ����½�
	FILE *writeFile = fopen(oFileName, "wb");

	if (kind > 1)
	{
		//д���ַ�������
		fwrite(&kind, sizeof(int), 1, writeFile);

		//д����ַ�����Ƶ��
		for (int i = 0; i < kind; ++i)
		{
			fwrite(&freqTable[i].uch, sizeof(unsigned char), 1, writeFile);
			fwrite(&freqTable[i].freq, sizeof(unsigned int), 1, writeFile);
		}
		//д��������ļ��ĳ��ȣ����ļ����ַ���
		fwrite(&fileSize, sizeof(unsigned int), 1, writeFile);

		//���������������Ϊ256�����Ե����ַ��ı����Ϊ255��
		//ͬʱ�����ǵ���һ�ζ�ȡ��Ϊ7�����ڶ��ζ�ȡ��Ϊ255�����������
		char codeBuffer[265] = "\0";

		unsigned char temp;

		FILE *readFile = fopen(iFileName, "rb");

		fread(&temp, sizeof(unsigned char), 1, readFile);
		while (!feof(readFile))
		{
			int pos;
			//�����ַ�temp�ڱ�����е�λ��
			//������У��ַ�Ƶ�ʴ����ǰ
			for (pos = 0; pos < kind; ++pos)
			{
				if (codeTable[pos].uch == temp)
					break;
			}

			strcat_s(codeBuffer, codeTable[pos].code);

			//һ���ֽ��� 8 λ������8Ϊ����λ��
			//ͨ����λ��(<<)��λ��( | )��������������Ʊ������temp
			while (strlen(codeBuffer) >= 8)
			{
				temp = '\0';
				for (int k = 0; k < 8; ++k)
				{
					temp <<= 1;

					if (codeBuffer[k] == '1')
						temp |= 1;
				}

				//��temp����������ļ����൱�ڴ�������Ʊ���
				fwrite(&temp, sizeof(unsigned char), 1, writeFile);

				//���codeBuffer[]���Ѵ����ǰ8���ַ�
				strcpy_s(codeBuffer, codeBuffer + 8);
			}

			fread(&temp, sizeof(unsigned char), 1, readFile);
		}

		//����codeBuffer[]�������8λ�ı���
		int len = strlen(codeBuffer);

		if (len > 0)
		{
			int j;
			temp = '\0';

			for (j = 0; j < len; ++j)
			{
				temp <<= 1;

				if (codeBuffer[j] == '1')
					temp |= 1;
			}
			//��������λ��
			for (; j < 8; ++j)
			{
				temp <<= 1;
			}

			fwrite(&temp, sizeof(unsigned char), 1, writeFile);
		}

		fclose(readFile);
	}

	//��ֻ��һ���ַ�ʱ��ֱ�Ӵ洢
	else
	{
		fwrite(&kind, sizeof(int), 1, writeFile);
		fwrite(&freqTable[0].uch, sizeof(unsigned char), 1, writeFile);
		fwrite(&freqTable[0].freq, sizeof(unsigned int), 1, writeFile);
	}

	fclose(writeFile);

	cout << "\n�ļ�������ɣ�������Ϣ�Ѵ洢��" << oFileName << endl;
}

/*****************************************************************
������bool  decoding()
���ܣ���ȡ�ļ���Ϣ��������Ӧ��huffTree, ���ļ����н��룬
		   ���洢��ָ���ļ���;
����ֵ��true:��ȡ�ɹ��� false:��ȡʧ��
*****************************************************************/
bool HuffmanFileCode::decoding()
{
	unsigned char temp;

	FILE *writeFile = fopen(oFileName, "wb");

	FILE *readFile = fopen(iFileName, "rb");

	if (readFile == NULL)
		return false;

	fread(&kind, sizeof(int), 1, readFile);

	if (1 == kind)
	{
		fread(&temp, sizeof(unsigned char), 1, readFile);

		unsigned int freq;
		fread(&freq, sizeof(unsigned int), 1, readFile);

		for (unsigned int i = 1; i <= freq; ++i)
		{
			fwrite(&temp, sizeof(unsigned char), 1, writeFile);
		}
	}

	else
	{
		freqTable = new sCharFreq[kind];

		//��ȡƵ������Ϣ
		for (int i = 0; i < kind; ++i)
		{
			fread(&freqTable[i].uch, sizeof(unsigned char), 1, readFile);
			fread(&freqTable[i].freq, sizeof(unsigned int), 1, readFile);
		}
		//����Ƶ��������������
		creatHuffTree();

		fread(&fileSize, sizeof(unsigned int), 1, readFile);

		int num = 0;
		int node = 2 * kind - 2;  //��nodeָ��Huffman���ĸ����

		for (unsigned int i = 1; i <= fileSize; ++i)
		{
			fread(&temp, sizeof(unsigned char), 1, readFile);

			for (int j = 0; j < 8; ++j)
			{
				//128�Ķ�����Ϊ 1000 0000
				//ʹ�� λ������� & �����ж�temp�����λ�Ƿ�Ϊ 1
				if (temp & 128)
					node = huffTree[node].rchild;

				else
					node = huffTree[node].lchild;

				temp <<= 1;

				//�ж��Ƿ��ѵ����ⲿ���
				if (huffTree[node].lchild == -1 && huffTree[node].rchild == -1)
				{
					fwrite(&huffTree[node].uch, sizeof(unsigned char), 1, writeFile);

					node = 2 * kind - 2; //���´Ӹ�������²���

					++num;
					if (num == fileSize)
						goto end;
				}
			}
		}
	}

end:
	fclose(readFile);
	fclose(writeFile);

	cout << "\n�ļ�������ɣ�������Ϣ�Ѵ洢��" << oFileName << endl;
	return true;
}

/*****************************************************************
������void reset();
���ܣ������Ա�������㣬���³�ʼ��
*****************************************************************/
void HuffmanFileCode::reset()
{
	memset(iFileName, 0, sizeof(iFileName));
	memset(oFileName, 0, sizeof(oFileName));

	if (codeTable)
	{
		for (int i = 0; i < kind; ++i)
		{
				delete[] codeTable[i].code;
				codeTable[i].code = NULL;
		}
		delete[] codeTable;
		codeTable = NULL;
	}

	if (huffTree)
	{
		delete[] huffTree;
		huffTree = NULL;
	}
		

	if (freqTable)
	{
		delete[] freqTable;
		freqTable = NULL;
	}

	kind = 0;
	fileSize = 0;
}

/*****************************************************************
������void meau();
���ܣ����ܲ˵���������ص����Ա������ʵ��ָ������
*****************************************************************/
void HuffmanFileCode::meau()
{
	cout << "\n��ӭ�����������ļ�����ϵͳ��\n" << endl;

	cout << "\n---------------------------------------------------------------------\n" << endl;
	cout << "1:   �ļ�����" << '\t' << "2:   �ļ�����" << '\t' << "0:   �˳�\n" << endl;

	cout << "\n������Ӧ��������";

	int ope;

	while (cin >> ope)
	{
		switch (ope)
		{
		case 0:
			exit(0);

		case 1:
			getFilesName();

			if (!countCharFreq())
			{
				cout << "\n�����ļ������ڣ������²���!\n" << endl;
				memset(iFileName, 0, sizeof(iFileName));
				memset(oFileName, 0, sizeof(oFileName));
				break;
			}
			//showFreqTable();
			creatHuffTree();
			coding();
			//showCodeTable();
			saveCodes();
			reset();

			break;

		case 2:
			getFilesName();

			if (!decoding())
			{
				cout << "\n�����ļ������ڣ������²���!\n" << endl;
				memset(iFileName, 0, sizeof(iFileName));
				memset(oFileName, 0, sizeof(oFileName));
				break;
			}
			reset();
			break;

		default:
			cout << "\n��������ȷ��������\n" << endl;
			break;
		}
		cout << "\n\n------------------------------------------------------------------\n"<<endl;
		cout << "1:   �ļ�����" << '\t' << "2:   �ļ�����" << '\t' << "0:   �˳�\n" << endl;
		
		cout << "\n����������������������Ӧ��������";
	}
}