#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include "SourceVolume.h"
#include <iostream>
#include <fstream>
#include "cmdline.h"
#include "data2word.h"

using namespace std;

void readInfoFile(const std::string& infoFileName, int& data_number, std::string& datatype, hxy::my_int3& dimension, hxy::my_double3& space,
	vector<string>& file_list)
{
	file_list.clear();

	ifstream inforFile(infoFileName);

	inforFile >> data_number;
	inforFile >> datatype;
	inforFile >> dimension.x >> dimension.y >> dimension.z;
	//Debug 20190520 增加sapce接口
	inforFile >> space.x >> space.y >> space.z;
	const string filePath = infoFileName.substr(0, infoFileName.find_last_of('/') == -1 ?
		infoFileName.find_last_of('\\') + 1 : infoFileName.find_last_of('/') + 1);
	std::cout << (filePath.c_str()) << std::endl;
	for (auto i = 0; i < data_number; i++)
	{
		string rawFileName;
		inforFile >> rawFileName;
		string volumePath = filePath + rawFileName;
		file_list.push_back(volumePath);
	}
	std::cout << "Info file name : \t\t" << infoFileName.c_str() << std::endl;
	std::cout << "Volume number : \t\t" << data_number << std::endl;
	std::cout << "data type : \t\t\t" << datatype.c_str() << std::endl;
	std::cout << "Volume dimension : \t\t" << "[" << dimension.x << "," << dimension.y << "," << dimension.z << "]" << std::endl;
	std::cout << "Space dimension : \t\t" << "[" << space.x << "," << space.y << "," << space.z << "]" << std::endl;
	for (auto i = 0; i < data_number; i++)
	{
		std::cout << "Volume " << i << " name : \t\t" << file_list[i].c_str() << std::endl;
	}


	std::cout << "Info file has been loaded successfully." << std::endl;

}

int main(int argc, char **argv) {

	if(argc<2)
	{
		printf("Error : Please provide command line arguments.\n");
		//return 0;
	}

	// create a parser
	cmdline::parser a;

	a.add<string>("vifo_path", 'p', "vifo file path", true, "");

	a.add<string>("output_ww", 'w', "output word-word net file path", false, "./workspace/ww.net");

	a.add<string>("output_words", 'W', "output words node file path", false, "./workspace/word.node");

	a.add<int>("window_size", 's', "window size for creating word-word net", false, 3,
		cmdline::oneof<int>(1, 3));

	a.add<int>("min_count", 'm', "minimum count for filtering vertex", false, 0,
		cmdline::range<int>(0, 1000000));

	if(argc>=2)
	{
		a.parse_check(argc, argv);
	}
	


	string			infoFileName = argc < 2 ? "F:\\atmosphere\\timestep21_float\\_SPEEDf21.vifo":a.get<string>("vifo_path");
	int				data_number;
	string			datatype;
	hxy::my_int3	dimension;
	hxy::my_double3	space;
	vector<string>	file_list;

	readInfoFile(infoFileName, data_number, datatype, dimension, space, file_list);
	auto file_path = file_list[0].substr(0, file_list[0].find_last_of('.'));

	SourceVolume source_volume(file_list, dimension.x, dimension.y, dimension.z, datatype);

	//source_volume.loadVolume();	//origin data
	source_volume.loadRegularVolume(); //[0, 255] data

	auto volume_data = source_volume.getRegularVolume(0);

	Volume2Word volume2word;
	int window_size = argc < 2 ? 3 : a.get<int>("window_size");
	volume2word.process((*volume_data).data(), dimension.x, dimension.y, dimension.z, window_size);
	auto word_word_net_file_path = argc < 2 ?"./workspace/ww.net": a.get<string>("output_ww");
	auto words_node_file_path = argc < 2 ? "./workspace/word.node" : a.get<string>("output_words");
	volume2word.saveNet(word_word_net_file_path);
	volume2word.saveWords(words_node_file_path);

	return 0;
}