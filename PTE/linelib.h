#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <string>
#include <Eigen/Dense>
#include <iostream>

#define MAX_STRING 10000
#define EXP_TABLE_SIZE 1000
#define MAX_EXP 6
const int neg_table_size = 1e8;
const int hash_table_size = 30000000;

typedef float real;

typedef Eigen::Matrix< real, Eigen::Dynamic,
	Eigen::Dynamic, Eigen::RowMajor | Eigen::AutoAlign >
	BLPMatrix;

typedef Eigen::Matrix< real, 1, Eigen::Dynamic,
	Eigen::RowMajor | Eigen::AutoAlign >
	BLPVector;

//节点，仅存储一个节点的名字（单词）
struct struct_node {
	char *word;
};
//存储相邻边信息
struct hin_nb {
	int nb_id;		//neighbor index
	double eg_wei;	//edge weight
	char eg_tp;		//edge type (one of 'l' 'w' and 'd')
};

class sampler
{
	long long n;
	long long *alias;
	double *prob;

public:
	sampler();
	~sampler();

	void init(long long ndata, double *p);
	long long draw(double ran1, double ran2);
};

class line_node
{
protected:
	struct struct_node *node;					//存储每个节点对应的名称
	int node_size, node_max_size, vector_size;	//所有节点的多少，节点最大size，向量维度
	char node_file[MAX_STRING];					//节点对应的文件
	int *node_hash;								//节点对应的hash数组
	real *_vec;									//节点对应的向量?
	Eigen::Map<BLPMatrix> vec;					//?

	int get_hash(char *word);
	int add_node(char *word);
public:
	line_node();
	~line_node();

	friend class line_hin;
	friend class line_trainer;

	void init(char *file_name, int vector_dim);
	int search(char *word);
	void output(char *file_name, int binary);
};

class line_hin
{
protected:
	char hin_file[MAX_STRING];			//读取的组合ww.net, dw.net, lw.net的text.hin文件，包含了所有节点的连接信息，通过type区分

	line_node *node_u, *node_v;			//连接的两个网络，其中u表示网络中所有节点，v仅表示单词节点
	std::vector<hin_nb> *hin;			//hin是一个二维数组，第一维度大小为所有节点的个数size(node_u)，第二维度存储每个node_u节点连接node_v节点的信息
	long long hin_size;					//hin_size保存了总连接节点的个数

public:
	line_hin();
	~line_hin();

	friend class line_trainer;

	void init(char *file_name, line_node *p_u, line_node *p_v);
};

class line_trainer
{
protected:
	line_hin *phin;

	int *u_nb_cnt; int **u_nb_id; double **u_nb_wei;
	double *u_wei, *v_wei;

	//smp_u存储了node_u网络中每个节点权重之和转换的概率
	//smp_u_nb是一个一维数组，长度为node_size，每个index存储每个节点连接其他节点的权重转换的概率
	sampler smp_u, *smp_u_nb;
	real *expTable;
	int neg_samples, *neg_table;

	char edge_tp;
public:
	line_trainer();
	~line_trainer();

	void init(char edge_type, line_hin *p_hin, int negative);
	void train_sample(real alpha, real *_error_vec, double(*func_rand_num)(), unsigned long long &rand_index);
};