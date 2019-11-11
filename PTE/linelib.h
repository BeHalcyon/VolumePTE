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

//�ڵ㣬���洢һ���ڵ�����֣����ʣ�
struct struct_node {
	char *word;
};
//�洢���ڱ���Ϣ
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
	struct struct_node *node;					//�洢ÿ���ڵ��Ӧ������
	int node_size, node_max_size, vector_size;	//���нڵ�Ķ��٣��ڵ����size������ά��
	char node_file[MAX_STRING];					//�ڵ��Ӧ���ļ�
	int *node_hash;								//�ڵ��Ӧ��hash����
	real *_vec;									//�ڵ��Ӧ������?
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
	char hin_file[MAX_STRING];			//��ȡ�����ww.net, dw.net, lw.net��text.hin�ļ������������нڵ��������Ϣ��ͨ��type����

	line_node *node_u, *node_v;			//���ӵ��������磬����u��ʾ���������нڵ㣬v����ʾ���ʽڵ�
	std::vector<hin_nb> *hin;			//hin��һ����ά���飬��һά�ȴ�СΪ���нڵ�ĸ���size(node_u)���ڶ�ά�ȴ洢ÿ��node_u�ڵ�����node_v�ڵ����Ϣ
	long long hin_size;					//hin_size�����������ӽڵ�ĸ���

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

	//smp_u�洢��node_u������ÿ���ڵ�Ȩ��֮��ת���ĸ���
	//smp_u_nb��һ��һά���飬����Ϊnode_size��ÿ��index�洢ÿ���ڵ����������ڵ��Ȩ��ת���ĸ���
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