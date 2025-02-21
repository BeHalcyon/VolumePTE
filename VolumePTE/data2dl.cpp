#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <map>
#include <string>
#define MAX_STRING 10000

const int hash_table_size = 30000000;

struct ClassVertex {
	double degree;
	char *name;
};

char text_file[MAX_STRING], label_file[MAX_STRING], output_label_word[MAX_STRING], output_doc_word[MAX_STRING], output_docs[MAX_STRING], output_labels[MAX_STRING];
struct ClassVertex *vertex;
int min_count = 5;
int *vertex_hash_table;
int max_num_vertices = 1000, num_vertices = 0, doc_size = 0;

//doc存储每个句子，对应的单词id
std::vector< std::vector<int> > doc;
//doc_label存储句子的类型
std::vector<std::string> doc_label;
//对应label处理过后，将映射为1
std::map<std::string, int> label2flag;
int label_size = 0;

unsigned int Hash(char *key)
{
	unsigned int seed = 131;
	unsigned int hash = 0;
	while (*key)
	{
		hash = hash * seed + (*key++);
	}
	return hash % hash_table_size;
}

void InitHashTable()
{
	vertex_hash_table = (int *)malloc(hash_table_size * sizeof(int));
	for (int k = 0; k != hash_table_size; k++) vertex_hash_table[k] = -1;
}

void InsertHashTable(char *key, int value)
{
	int addr = Hash(key);
	while (vertex_hash_table[addr] != -1) addr = (addr + 1) % hash_table_size;
	vertex_hash_table[addr] = value;
}

int SearchHashTable(char *key)
{
	int addr = Hash(key);
	while (1)
	{
		if (vertex_hash_table[addr] == -1) return -1;
		if (!strcmp(key, vertex[vertex_hash_table[addr]].name)) return vertex_hash_table[addr];
		addr = (addr + 1) % hash_table_size;
	}
	return -1;
}

/* Add a vertex to the vertex set */
int AddVertex(char *name)
{
	int length = strlen(name) + 1;
	if (length > MAX_STRING) length = MAX_STRING;
	//针对每个节点，动态申请内存
	vertex[num_vertices].name = (char *)calloc(length, sizeof(char));
	strcpy(vertex[num_vertices].name, name);
	//添加到词汇表中后，每个词都是一个图的节点，初始化节点度为0
	vertex[num_vertices].degree = 0;
	num_vertices++;
	//如果超出内存限制，动态修改
	if (num_vertices + 2 >= max_num_vertices)
	{
		max_num_vertices += 1000;
		vertex = (struct ClassVertex *)realloc(vertex, max_num_vertices * sizeof(struct ClassVertex));
	}
	//将该节点映射到Hash Table上
	InsertHashTable(name, num_vertices - 1);
	return num_vertices - 1;
}
/***
 *	根据输入的文档，构建词汇表。
 *
 */
void BuildVocab()
{
	vertex = (struct ClassVertex *)calloc(max_num_vertices, sizeof(struct ClassVertex));
	vertex_hash_table = (int *)malloc(hash_table_size * sizeof(int));
	for (int k = 0; k != hash_table_size; k++) vertex_hash_table[k] = -1;

	char word[MAX_STRING];
	FILE *fin;
	//cnt记录字符的个数
	long long cnt = 0;
	fin = fopen(text_file, "rb");
	//全局变量，记录节点的个数
	num_vertices = 0;
	AddVertex((char *)"</s>");
	while (1)
	{
		//获取一个单词长度，遇到空格、tab、换行结束
		if (fscanf(fin, "%s", word) != 1) break;
		cnt++;
		if (cnt % 100000 == 0) {
			printf("%lldK%c", cnt / 1000, 13);
			fflush(stdout);
		}
		//将单词添加到词汇表汇总
		if (SearchHashTable(word) == -1) AddVertex(word);
	}
	printf("Number of tokens: %lld\n", cnt);
	printf("Number of words: %d\n", num_vertices);
	fclose(fin);
}


/***
 * 从输入流中读取一个单词
 */
void ReadWord(char *word, FILE *fin) {
	int a = 0, ch;
	while (!feof(fin)) {
		ch = fgetc(fin);
		if (ch == 13) continue;
		if ((ch == ' ') || (ch == '\t') || (ch == '\n')) {
			if (a > 0) {
				if (ch == '\n') ungetc(ch, fin);
				break;
			}
			if (ch == '\n') {
				strcpy(word, (char *)"</s>");
				return;
			}
			else continue;
		}
		word[a] = ch;
		a++;
		if (a >= MAX_STRING - 1) a--;   // Truncate too long words
	}
	word[a] = 0;
}

/***
 *	从text_file和label_file中读取数据
 *	label_file中每个label占据一行
 *	text_file中每个句子占据一行
 */
void ReadData()
{
	FILE *fi, *fil;
	int curword;
	char word[MAX_STRING], curlabel[MAX_STRING];
	std::vector<int> vt;

	fi = fopen(text_file, "rb");
	fil = fopen(label_file, "rb");
	while (1)
	{
		//
		if (fscanf(fil, "%s", curlabel) != 1) break;

		//vt记录每个label对应的句子的所有单词的id
		vt.clear();
		while (1)
		{
			ReadWord(word, fi);
			curword = SearchHashTable(word);
			if (curword == -1) continue;
			vt.push_back(curword);
			if (curword == 0) break;
		}
		//doc_size记录label的数量
		doc_size++;
		
		doc_label.push_back(curlabel);
		
		doc.push_back(vt);
		
		label2flag[curlabel] = 1;
	}
	fclose(fi);
	printf("Number of docs: %d\n", doc_size);
}

void Process()
{
	BuildVocab();
	ReadData();

	FILE *fo;
	if (output_label_word[0] != 0)
	{
		/***
		 * 以二进制形式存储label_word文件，文件格式："
		 * label:1 hello 1 l
		 * label:1 world 1 l
		 * label:2 xiangyanghe 1 l
		 * label:2 and 1 l
		 * label:2 CAD&CG
		 * "
		 * 存储格式：label_name word_name edge_weight edge_type('l' for label-word, 'd' for doc-word)
		 */
		char lb[MAX_STRING];

		fo = fopen(output_label_word, "wb");
		//以label为数量循环
		for (int m = 0; m != doc_size; m++)
		{
			strcpy(lb, doc_label[m].c_str());
			int len = doc[m].size();
			for (int n = 0; n != len; n++)
				fprintf(fo, "label:%s %s 1 l\n", lb, vertex[doc[m][n]].name);
		}
		fclose(fo);
		/***
		 * 以二进制形式存储非重复label文件，文件格式："
		 * label:1
		 * label:2
		 * label:3
		 * "
		 */
		std::map<std::string, int>::iterator iter;

		fo = fopen(output_labels, "wb");
		for (iter = label2flag.begin(); iter != label2flag.end(); iter++) fprintf(fo, "label:%s\n", (iter->first).c_str());
		fclose(fo);
	}
	if (output_doc_word[0] != 0)
	{
		/***
		 * 以二进制形式存储doc_word文件，表示第i个文件与对应单词的文件，文件格式："
		 * doc:1 hello 1 d
		 * doc:1 world 1 d
		 * doc:2 xiangyanghe 1 d
		 * doc:2 and 1 d
		 * doc:2 CAD&CG 1 d
		 * "
		 */
		fo = fopen(output_doc_word, "wb");
		for (int m = 0; m != doc_size; m++)
		{
			int len = doc[m].size();
			for (int n = 0; n != len; n++)
				fprintf(fo, "doc:%d %s 1 d\n", m, vertex[doc[m][n]].name);
		}
		fclose(fo);
		/***
		 * 以二进制形式存储doc文件，文件格式："
		 * doc:1
		 * doc:2
		 * doc:3
		 * "
		 */
		fo = fopen(output_docs, "wb");
		for (int m = 0; m != doc_size; m++) fprintf(fo, "doc:%d\n", m);
		fclose(fo);
	}
}

int ArgPos(char *str, int argc, char **argv) {
	int a;
	for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
		if (a == argc - 1) {
			printf("Argument missing for %s\n", str);
			exit(1);
		}
		return a;
	}
	return -1;
}

int main(int argc, char **argv) {
	int i;
	if (argc == 1) {
		return 0;
	}
	output_label_word[0] = 0;
	output_doc_word[0] = 0;
	if ((i = ArgPos((char *)"-text", argc, argv)) > 0) strcpy(text_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-label", argc, argv)) > 0) strcpy(label_file, argv[i + 1]);
	if ((i = ArgPos((char *)"-output-lw", argc, argv)) > 0) strcpy(output_label_word, argv[i + 1]);
	if ((i = ArgPos((char *)"-output-dw", argc, argv)) > 0) strcpy(output_doc_word, argv[i + 1]);
	if ((i = ArgPos((char *)"-output-labels", argc, argv)) > 0) strcpy(output_labels, argv[i + 1]);
	if ((i = ArgPos((char *)"-output-docs", argc, argv)) > 0) strcpy(output_docs, argv[i + 1]);
	if ((i = ArgPos((char *)"-min-count", argc, argv)) > 0) min_count = atoi(argv[i + 1]);
	Process();
	return 0;
}