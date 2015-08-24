#include <iostream>
#include "SynthStream.h"

using namespace std;

void main(int argc, char * argv[])
{
	if (argc == 5 && strcmp(argv[1], "-e") == 0)
	{
		fstream fs;
		string data;

		fs.open(argv[2], ios_base::in | ios_base::out | ios_base::binary);
		if (!fs.is_open())
		{
			fs.open(argv[2], ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc);
		}
		while (fs.good())
		{
			data += fs.get();
		}
		SynthStream synth(argv[3], argv[4], 'i');
		//synth.write("Lorem ipsum dolor sit amet, consectetur adipiscing elit.\0", 100);
		synth.write(data.c_str(), data.length());
		synth.flush();
		synth.close();
		printf("File Divided.");
		std::cin.get();
	}
	else if (argc == 5 && strcmp(argv[1], "-d") == 0)
	{
		fstream fs;
		SynthStream synth(argv[2], argv[3], 'i');
		string data;

		fs.open(argv[4], ios_base::in | ios_base::out | ios_base::binary);
		if (!fs.is_open())
		{
			fs.open(argv[4], ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc);
		}
		while (!synth.eof())
		{
			data += synth.get();
		}
		synth.close();
		printf("Number of characters found: %d\n", data.length());
		//printf(data.c_str());	//prints to screen.
		fs.write(data.c_str(), data.length());
		std::cin.get();
	}
	else if (argc == 3)
	{
		SynthStream synth(argv[1], argv[2], 'i');
		string data;
		while (!synth.eof())
		{
			data += synth.get();
		}
		synth.close();
		printf("Number of characters found: %d\n", data.length());
		printf(data.c_str());
		std::cin.get();
	}
	else
	{
		//runs if other conditions are not met.
		cout << "Usage: taiji [-e/-d] file1 file2 file3\n"
			<< "-e: file1 -> file2 & file3"
			<< "-d: file1 & file2 -> file3"
			<< "or to just read: taiji inputFile1 inputFile2";
		std::cin.get();
	}
	return;
}