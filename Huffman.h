
//�����ַ�Ƶ����ṹ��
struct sCharFreq
{
	unsigned char uch;   //�洢�ַ�
	unsigned int freq;   //�洢�ַ������ļ��г���Ƶ��
};

//����Huffman���Ľ��Ĵ洢�ṹ
struct huffNode
{
	unsigned char uch;        
	unsigned int freq;                 
	int lchild;
	int rchild;
	int father;
};

//����Huffman�����洢�ṹ
struct huffCode
{
	unsigned char uch;      //��Ӧ�ַ�
	char  *code; //�ַ���Ӧ�Ķ����Ʊ��루ʹ��ָ�룬ָ��̬������ڴ棩
};

class HuffmanFileCode
{
private:
	int kind;    //�ַ�����
	unsigned int fileSize;        //�����ļ���С
	sCharFreq *freqTable;     //�ַ�Ƶ����
	huffNode *huffTree;
	huffCode *codeTable;
	char iFileName[260];   //windows���ļ������Ϊ255���ַ�
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
	void reset();  //�ͷŶ�̬������ڴ�ռ䣬�����Ա�������³�ʼ��
	
public:
	HuffmanFileCode();
	void meau();  //���ܲ˵���������ص����Ա������ʵ��ָ������
};