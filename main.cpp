#include <iostream>
#include "taiji.h"

using namespace std;


void main(int argc, char * argv[])
{
	if (argc == 5 && strcmp(argv[1], "-e") == 0)
	{
		int error = taiji_encode_to_files(argv[2], argv[3], argv[4], 'i');
		if (error == 0)
		{
			printf("File divided.\n");
		}
		else
		{
			printf("Error encountered.\n");
		}
		std::cin.get();
	}
	else if (argc == 5 && strcmp(argv[1], "-d") == 0)
	{
		int error = taiji_decode_from_files(argv[2], argv[3], argv[4], 'i');
		if (error == 0)
		{
			printf("File synthesized\n");
		}
		else
		{
			printf("Error encountered.\n");
		}
		std::cin.get();
	}
	else if (argc == 3)
	{
		string data;
		int error = taiji_decode(argv[1], argv[2], data, 'i');
		if (error == 0)
		{
			printf("Number of characters found: %d\n", data.length());
			printf(data.c_str());
		}
		else
		{
			printf("Error encountered.\n");
		}
		
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