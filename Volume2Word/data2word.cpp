

#include "data2word.h"
using namespace std;

Volume2Word::Volume2Word()
{
}

Volume2Word::~Volume2Word()
{
	for (auto i = 0; i < neighbor_histogram.size(); i++)
	{
		neighbor_histogram.clear();
	}
	neighbor_histogram.clear();

	vertex.clear();
	vertex_hash_table.clear();
}

std::vector<std::vector<int>> Volume2Word::getNeighborHistogram() const
{
	return neighbor_histogram;
}

void Volume2Word::initVocab()
{
	//Initialize 
	vertex.clear();
	vertex_hash_table.clear();
	neighbor_histogram.clear();

	vertex.resize(max_num_vertices);
	vertex_hash_table.resize(HASH_TABLE_SIZE, -1);
	neighbor_histogram.resize(HISTOGRAM_SIZE);
	for (auto& i : neighbor_histogram)
		i.resize(HISTOGRAM_SIZE, 0);
	//vertex = static_cast<struct ClassVertex *>(calloc(max_num_vertices, sizeof(struct ClassVertex)));
	//vertex_hash_table = (int *)malloc(HASH_TABLE_SIZE * sizeof(int));
	//for (int k = 0; k < HASH_TABLE_SIZE; k++) vertex_hash_table[k] = -1;
	num_vertices = 0;
}


int Volume2Word::getWordHash(const int intword)
{
	return intword;
}

/* Add a vertex to the vertex set */
int Volume2Word::addWordToVocab(int intword)
{
	vertex[num_vertices].intword = intword;
	//vocab[vocab_size].cn = 0;
	num_vertices++;

	const unsigned int hash = getWordHash(intword);

	vertex_hash_table[hash] = num_vertices - 1;

	return num_vertices - 1;
}
int Volume2Word::searchVocab(const int intword) {
	const unsigned int hash = getWordHash(intword);
	return vertex_hash_table[hash];
}

void Volume2Word::buildVocab(unsigned char* regular_data, const int & width, const int & height, const int & depth)
{
	const auto sz = width * height * depth;

	for (auto i = 0; i < sz; i++)
	{
		auto& intword = regular_data[i];
		if (i % 10000 == 0) {
			printf("Loading data to vocab: %f%%%c", i*1.0f / sz * 100, 13);
			fflush(stdout);
		}
		const auto index = searchVocab(intword);
		if (index < 0) {
			addWordToVocab(intword);
		}
	}
	printf("\n");
	printf("Number of tokens: %d\n", sz);
	printf("Number of words: %d\n", num_vertices);
}

void Volume2Word::calcNeighborHistogram(unsigned char* regular_data, const int & width, const int & height, const int & depth, const int & window_size)
{
	long long word_index = 0;

	auto currentIndex = 0;
	
	const auto& xDim = width;
	const auto& yDim = height;
	const auto& zDim = depth;

	const auto sz = xDim * yDim*zDim;

	
	if(window_size==3)
	{
		const int dx26[26] = { -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1 };
		const int dy26[26] = { -1, -1, -1,  0,  0,  0,  1,  1,  1, -1, -1, -1,  0,  0,  1,  1,  1, -1, -1, -1,  0,  0,  0,  1,  1,  1 };
		const int dz26[26] = { -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1 };

		for (auto index = 0; index < sz; index++)
		{
			const int oz = index / (xDim*yDim);
			const int ox = index % xDim;
			const int oy = (index % (xDim*yDim)) / xDim;

			auto center_word = regular_data[index];
			for (auto i = 0; i < 26; i++)
			{
				int nx = ox + dx26[i];//new x
				int ny = oy + dy26[i];//new y
				int nz = oz + dz26[i];//new z

				if (nx >= 0 && nx < xDim && ny >= 0 && ny < yDim && nz >= 0 && nz < zDim)
				{
					int nind = nz * xDim*yDim + ny * xDim + nx;

					auto& int_word = regular_data[nind];
					//if (int_word<0 || int_word>HISTOGRAM_SIZE) continue;
					if (vertex_hash_table[int_word] < 0) continue;

					neighbor_histogram[center_word][int_word]++;
				}
			}
		}
	}
	else if(window_size == 1)
	{
		const int dx6[6] = { -1,  1,  0,  0,  0,  0 };
		const int dy6[6] = { 0,  0, -1,  1,  0,  0 };
		const int dz6[6] = { 0,  0,  0,  0, -1,  1 };

		for (auto index = 0; index < sz; index++)
		{
			const int oz = index / (xDim*yDim);
			const int ox = index % xDim;
			const int oy = (index % (xDim*yDim)) / xDim;

			auto center_word = regular_data[index];
			for (auto i = 0; i < 6; i++)
			{
				int nx = ox + dx6[i];//new x
				int ny = oy + dy6[i];//new y
				int nz = oz + dz6[i];//new z

				if (nx >= 0 && nx < xDim && ny >= 0 && ny < yDim && nz >= 0 && nz < zDim)
				{
					int nind = nz * xDim*yDim + ny * xDim + nx;

					auto& int_word = regular_data[nind];
					//if (int_word<0 || int_word>HISTOGRAM_SIZE) continue;
					if (vertex_hash_table[int_word] < 0) continue;

					neighbor_histogram[center_word][int_word]++;
				}
			}
		}
	}
	else
	{
		printf("Error : Window size is not available.\n");
		exit(-1);
	}
}


void Volume2Word::saveWords(const std::string & output_words_file)
{
	/***
	* 以二进制形式存储word文件，即单词库，词典："
	* hello world 123
	* hello everyone 345
	* "
	*/
	FILE* fo = fopen(output_words_file.c_str(), "w");
	for (int k = 0; k < num_vertices; k++) 
		fprintf(fo, "%s\n", std::to_string(vertex[vertex_hash_table[k]].intword).c_str());
	fclose(fo);
}

// window_size = 3 for 26 voxels and = 1 for 6 voxels
void Volume2Word::process(unsigned char* regular_data, const int & width, const int & height, const int & depth, const int & window_size)
{
	initVocab();

	buildVocab(regular_data, width, height, depth);

	calcNeighborHistogram(regular_data, width, height, depth, window_size);

	is_calculated = true;

}

void Volume2Word::saveNet(const std::string& output_net_file)
{

	/***
	* 以二进制形式存储word_word文件，文件格式："
	* hello world 123
	* hello everyone 345
	* "
	*/
	FILE *fo = fopen(output_net_file.c_str(), "wb");
	auto cnt = 0;
	for (auto i = 0; i < HISTOGRAM_SIZE; i++)
	{
		for (auto j = 0; j < HISTOGRAM_SIZE; j++)
		{
			if (cnt % 1000 == 0)
			{
				printf("%cWrite word-word file: %.3lf%%", 13, double(cnt) / (HISTOGRAM_SIZE*HISTOGRAM_SIZE) * 100);
				fflush(stdout);
			}
			if (neighbor_histogram[i][j] > 0)
			{
				fprintf(fo, "%s\t%s\t%d\t\tw\n", 
					std::to_string(i).c_str(), std::to_string(j).c_str(), neighbor_histogram[i][j]);
			}
			cnt++;
		}
	}
	printf("\n");
	fclose(fo);

	printf("The ww.net file has been saved.");
}