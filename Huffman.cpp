
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

//定义无符号int 的最大值
//f =15, 转换为二进制是 1111
#define MAX 0xffffffff

/*****************************************************************
函数：HuffmanFileCode(）
功能： 为HuffmanFileCode类的构造函数，初始化成员变量；
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
函数(内联）： void getFilesName()
功能： 得到从键盘输入的输入文件和输出文件的路径及名称；
*****************************************************************/
inline void HuffmanFileCode::getFilesName()
{
	cout << "\n请输入要操作的文件路径及名称：";
	cin >> iFileName;

	cout << "\n请输入要输出的文件的路径及名称：";
	cin >> oFileName;

	cout << endl;
}

//定义比较函数，将表freqTable[]按字符频数降序排
bool cmpFreq(sCharFreq x, sCharFreq y)
{
	return x.freq > y.freq;
}

/*****************************************************************
函数：bool countCharFreq()
功能：求出字符种类kind, 文件大小fileSize, 以及频数表freqTableC[];
返回值：true：成功读取； false：读取失败
*****************************************************************/
bool HuffmanFileCode::countCharFreq()
{
	//以二进制的方式将文件内容照搬读取
	//若用“r", 则但遇到换行“\t\n"时，会自动解析为"\n"读取
	FILE *readFile = fopen(iFileName, "rb");

	//当文件不存在时
	if (NULL == readFile) {
		return false;
	}

	freqTable = new sCharFreq[256];

	//初始化频数表
	for (int i = 0; i < 256; ++i)
	{
		freqTable[i].uch = (unsigned char)i;
		freqTable[i].freq = 0;
	}

	//每次使用时初始化
	kind = 0;
	fileSize = 0;

	fseek(readFile, 0L, SEEK_END);   //将文件指针设置到文件尾
	fileSize = ftell(readFile);            //读取文件大小
	fseek(readFile, 0L, SEEK_SET);   //将文件指针重新定义的到文件开头

	unsigned char temp;

	/*
	   对于feof(),在读完文件的最后一个字符后，readFile->flag仍然没有被置为_IOEOF，因而feof()仍然没有探测到文件结尾。
	   直到再次调用fread()执行读操作，feof()才能探测到文件结尾。这样就多执行了一次。所以应该先读取再判断。
	*/
	fread(&temp, sizeof(unsigned char), 1, readFile);
	while (!feof(readFile))
	{
		//统计字符temp出现的频数
		++freqTable[temp].freq;

		//从文件中读取一个字节到temp中（unsigned char 占一个字节）
		fread(&temp, sizeof(unsigned char), 1, readFile);
	}

	//将表freqTable[]按字符频数降序排
	sort(freqTable, freqTable + 256, cmpFreq);

	int i;
	//求得字符种类数
	for (i = 0; freqTable[i].freq > 0; ++i);

	kind = i;

	fclose(readFile);

	return true;
}

/*****************************************************************
函数：void showCharFreq()
功能：打印freqTableC[]里的内容
*****************************************************************/
void HuffmanFileCode::showFreqTable()
{
	cout << "\n统计得到的字符频数表如下：\n" << endl;
	cout << right << setw(6) << "字符" << '\t' << left << setw(32) << "频数\n" << endl;

	for (int i = 0; i < kind; ++i)
	{
		cout << right << setw(6) << freqTable[i].uch << '\t' << left << setw(32) << freqTable[i].freq << endl;
	}

	cout << "\n字符频数表打印完毕。\n" << endl;
}

/*****************************************************************
函数：void selectMins(huffNode *pt, int &lc, int &rc)
参数：指向哈夫树数组的pt指针，int型的查找长度len,
		  两个int型的左右孩子序号的引用；
功能：选出pt指向的huffNode[]中最小的两个结点，并返回其序号；
*****************************************************************/
void HuffmanFileCode::selectMins(huffNode *pt, int len, int *lc, int *rc)
{
	unsigned int min = MAX;
	int pos = 0;
	int i;

	//筛选出freq值最小的结点，将序号赋给lc
	for (i = 0; i < len; ++i)
	{
		//当pt[i].parent == 0 表示该结点还未插入树中
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
函数：void creatHuffTree()
功能：根据字符频数表freqTable[]，及种类数kind,
		   当kind>1时， 创建哈夫曼树huffTree[];
*****************************************************************/
void HuffmanFileCode::creatHuffTree()
{
	//扩充二叉树中，当有n个外部结点是，就有n-1个内部结点
	//字符种类数kind即为外部结点的个数
	huffTree = new huffNode[2 * kind - 1];

	int i;
	//初始化外部结点(按字符频率由大到小）
	for (i = 0; i < kind; ++i)
	{
		huffTree[i].uch = freqTable[i].uch;
		huffTree[i].freq = freqTable[i].freq;
		huffTree[i].lchild = huffTree[i].rchild = -1;
		huffTree[i].father = -1;
	}

	int lc, rc;
	//构建内部结点，建立哈夫曼树
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
函数：void  coding()
功能：根据生成的huffTree, 对文件进行编译，得到编码表codeTable
*****************************************************************/
void HuffmanFileCode::coding()
{
	codeTable = new huffCode[kind];

	int fa;
	int son;
	int codeSize;
	stack<char>codeStack;   //创建栈, 用于存储字符编码

	for (int i = 0; i < kind; ++i)
	{
		son = i;
		fa = huffTree[son].father;

		//从外部结点开始，向根结点上溯，将编码存入栈中
		//当fa == -1 ,则表示到达根结点
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

		//将栈中的编码信息存入编码表中
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
功能：打印编码表中的信息
*****************************************************************/
void HuffmanFileCode::showCodeTable()
{
	if (kind > 1)
	{
		cout << "\n统计得到的编码表如下：\n" << endl;
		cout << right << setw(6) << "字符" << '\t' << "编码\n" << endl;

		for (int i = 0; i < kind; ++i)
		{
			cout << right << setw(6) << codeTable[i].uch << '\t' << codeTable[i].code << endl;
		}

		cout << "\n编码表打印完毕。\n" << endl;
	}
	else
		cout << "\n文件中仅有一个字符，编码表为空，无法显示。\n" << endl;
}

/*****************************************************************
函数：void  saveCode()
功能：根据编码表将文件的编码信息以二进制形式存储到指定的文件中
*****************************************************************/
void HuffmanFileCode::saveCodes()
{
	//以二进制形式打开要输出的文件，若不存在，则新建
	FILE *writeFile = fopen(oFileName, "wb");

	if (kind > 1)
	{
		//写入字符种类数
		fwrite(&kind, sizeof(int), 1, writeFile);

		//写入各字符及其频数
		for (int i = 0; i < kind; ++i)
		{
			fwrite(&freqTable[i].uch, sizeof(unsigned char), 1, writeFile);
			fwrite(&freqTable[i].freq, sizeof(unsigned int), 1, writeFile);
		}
		//写入待编码文件的长度，及文件的字符数
		fwrite(&fileSize, sizeof(unsigned int), 1, writeFile);

		//哈夫曼树树高最大为256，所以单个字符的编码最长为255。
		//同时，考虑到第一次读取的为7，而第二次读取的为255的特殊情况。
		char codeBuffer[265] = "\0";

		unsigned char temp;

		FILE *readFile = fopen(iFileName, "rb");

		fread(&temp, sizeof(unsigned char), 1, readFile);
		while (!feof(readFile))
		{
			int pos;
			//查找字符temp在编码表中的位置
			//编码表中，字符频率大的在前
			for (pos = 0; pos < kind; ++pos)
			{
				if (codeTable[pos].uch == temp)
					break;
			}

			strcat_s(codeBuffer, codeTable[pos].code);

			//一个字节有 8 位，故以8为处理单位，
			//通过左位移(<<)与位或( | )运算符，将二进制编码存入temp
			while (strlen(codeBuffer) >= 8)
			{
				temp = '\0';
				for (int k = 0; k < 8; ++k)
				{
					temp <<= 1;

					if (codeBuffer[k] == '1')
						temp |= 1;
				}

				//将temp存入二进制文件，相当于存入二进制编码
				fwrite(&temp, sizeof(unsigned char), 1, writeFile);

				//清除codeBuffer[]中已存入的前8个字符
				strcpy_s(codeBuffer, codeBuffer + 8);
			}

			fread(&temp, sizeof(unsigned char), 1, readFile);
		}

		//处理codeBuffer[]中最后不足8位的编码
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
			//处理多余的位数
			for (; j < 8; ++j)
			{
				temp <<= 1;
			}

			fwrite(&temp, sizeof(unsigned char), 1, writeFile);
		}

		fclose(readFile);
	}

	//当只有一个字符时，直接存储
	else
	{
		fwrite(&kind, sizeof(int), 1, writeFile);
		fwrite(&freqTable[0].uch, sizeof(unsigned char), 1, writeFile);
		fwrite(&freqTable[0].freq, sizeof(unsigned int), 1, writeFile);
	}

	fclose(writeFile);

	cout << "\n文件编码完成，编码信息已存储到" << oFileName << endl;
}

/*****************************************************************
函数：bool  decoding()
功能：读取文件信息，生成相应的huffTree, 对文件进行解码，
		   并存储到指定文件中;
返回值：true:读取成功； false:读取失败
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

		//读取频数表信息
		for (int i = 0; i < kind; ++i)
		{
			fread(&freqTable[i].uch, sizeof(unsigned char), 1, readFile);
			fread(&freqTable[i].freq, sizeof(unsigned int), 1, readFile);
		}
		//根据频数表建立哈夫曼树
		creatHuffTree();

		fread(&fileSize, sizeof(unsigned int), 1, readFile);

		int num = 0;
		int node = 2 * kind - 2;  //将node指向Huffman树的根结点

		for (unsigned int i = 1; i <= fileSize; ++i)
		{
			fread(&temp, sizeof(unsigned char), 1, readFile);

			for (int j = 0; j < 8; ++j)
			{
				//128的二进制为 1000 0000
				//使用 位与运算符 & 可以判断temp的最高位是否为 1
				if (temp & 128)
					node = huffTree[node].rchild;

				else
					node = huffTree[node].lchild;

				temp <<= 1;

				//判断是否已到达外部结点
				if (huffTree[node].lchild == -1 && huffTree[node].rchild == -1)
				{
					fwrite(&huffTree[node].uch, sizeof(unsigned char), 1, writeFile);

					node = 2 * kind - 2; //重新从根结点向下查找

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

	cout << "\n文件解码完成，解码信息已存储到" << oFileName << endl;
	return true;
}

/*****************************************************************
函数：void reset();
功能：将类成员变量清零，重新初始化
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
函数：void meau();
功能：功能菜单，调用相关的类成员函数，实现指定操作
*****************************************************************/
void HuffmanFileCode::meau()
{
	cout << "\n欢迎来到哈夫曼文件编码系统！\n" << endl;

	cout << "\n---------------------------------------------------------------------\n" << endl;
	cout << "1:   文件编码" << '\t' << "2:   文件解码" << '\t' << "0:   退出\n" << endl;

	cout << "\n输入相应操作数：";

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
				cout << "\n操作文件不存在，请重新操作!\n" << endl;
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
				cout << "\n操作文件不存在，请重新操作!\n" << endl;
				memset(iFileName, 0, sizeof(iFileName));
				memset(oFileName, 0, sizeof(oFileName));
				break;
			}
			reset();
			break;

		default:
			cout << "\n请输入正确操作数。\n" << endl;
			break;
		}
		cout << "\n\n------------------------------------------------------------------\n"<<endl;
		cout << "1:   文件编码" << '\t' << "2:   文件解码" << '\t' << "0:   退出\n" << endl;
		
		cout << "\n如若继续操作，请输入相应操作数：";
	}
}