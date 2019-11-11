#pragma once
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <map>
struct ClassVertex {
	double degree = 0;
	//char *name;
	int intword = -1;
};
const int HASH_TABLE_SIZE = 10240;
const int HISTOGRAM_SIZE = 256;

//仅针对0~255的体数据
class Volume2Word
{
public:
	Volume2Word();

	~Volume2Word();
	
	void clear();
	void process(unsigned char* volume_data, const int & width, const int & height, const int & depth, const int & window_size);
	void saveNet(const std::string & output_net_file = "./workspace/ww.net");
	void saveWords(const std::string & output_words_file = "./workspace/word.node");
	std::vector<std::vector<int>> getNeighborHistogram() const;
protected:
	void initVocab();
	void buildVocab(unsigned char* volume_data, const int & width, const int & height, const int & depth);
	void calcNeighborHistogram(unsigned char* regular_data, const int & width, const int & height, const int & depth, const int & window_size);

	int getWordHash(int intword);
	int addWordToVocab(int intword);
	int searchVocab(int intword);

private:

	//int neighbor_histogram[HISTOGRAM_SIZE][HISTOGRAM_SIZE];
	std::vector<std::vector<int>>		neighbor_histogram;
	long long							totaltoken = 0;
	int									min_count = 0;
	int									window_size = 3;
	std::vector<struct ClassVertex>		vertex;
	std::vector<int>					vertex_hash_table;
	int									max_num_vertices = HISTOGRAM_SIZE;
	int									num_vertices = 0;
	bool								is_calculated = false;
};



inline void Volume2Word::clear()
{
	neighbor_histogram.clear(); 
	is_calculated = false;
}
