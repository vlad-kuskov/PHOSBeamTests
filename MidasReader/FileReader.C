#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

vector<string> readInfoFile(string filename, string key_run){

	vector<string> record;

	string run;
	string beam_energy;
	string comments;

	ifstream file;
	file.open(filename);

	bool found_run = false;

	while(getline(file, run, '\t') && !found_run){
		getline(file, beam_energy, '\t');
		getline(file, comments, '\n');
		if (run == key_run){
			found_run = true;
			record.push_back(run);
			record.push_back(beam_energy);
			record.push_back(comments);
		}

	}

	if (!found_run)
		record.push_back("0");

	return record;

}

