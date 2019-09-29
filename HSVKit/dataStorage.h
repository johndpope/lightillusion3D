#pragma once
#include "common.h"
#include<fstream>
#include<string>
class dataStorage {
public:
	string fileName;
	std::ofstream datafile;
	vector<string> loaddata;

public:
	dataStorage(string fileName) {
		std::ifstream ifs(fileName);
		string str;
		if (!ifs.fail()) {
			while (getline(ifs, str)) {
				loaddata.push_back(str);
			}
		}

		datafile = std::ofstream(fileName);
		for (int i = 0; i < loaddata.size(); i++) {
			datafile << loaddata[i] << endl;
		}
	};

	void writeData(float fps, vector<float> v,vector<float> u) {
		datafile << fps << std::endl;
		datafile << "static"<<",";
		for (int i = 0; i < v.size(); i++) {
			datafile << v[i] << ",";
		}
		datafile << std::endl;
		datafile << "move" << ",";
		for (int i = 0; i < u.size(); i++) {
			datafile << u[i] << ",";
		}
		datafile << std::endl;
	}

	void save() {
		datafile.close();
	}
};