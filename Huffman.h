
//定义字符频数表结构体
struct sCharFreq
{
	unsigned char uch;   //存储字符
	unsigned int freq;   //存储字符的在文件中出现频数
};

//定义Huffman树的结点的存储结构
struct huffNode
{
	unsigned char uch;        
	unsigned int freq;                 
	int lchild;
	int rchild;
	int father;
};

//定义Huffman编码表存储结构
struct huffCode
{
	unsigned char uch;      //相应字符
	char  *code; //字符对应的二进制编码（使用指针，指向动态申请的内存）
};

class HuffmanFileCode
{
private:
	int kind;    //字符种类
	unsigned int fileSize;        //编码文件大小
	sCharFreq *freqTable;     //字符频数表
	huffNode *huffTree;
	huffCode *codeTable;
	char iFileName[260];   //windows里文件名最多为255个字符
	char oFileName[260];

	void getFilesName();
	bool countCharFreq();
	void showFreqTable();
	void selectMins(huffNode *pt, int len, int *lc, int *rc);
	void creatHuffTree();
	void coding();
	void showCodeTable();
	void  saveCodes();
	bool decoding();
	void reset();  //释放动态申请的内存空间，将类成员变量重新初始化
	
public:
	HuffmanFileCode();
	void meau();  //功能菜单，调用相关的类成员函数，实现指定操作
};