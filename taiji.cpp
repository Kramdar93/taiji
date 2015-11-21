///////////////////////////
// (c) 2015 Mark Elsinger
// taiji implementation
// Implementation of said functions.
///////////////////////////

#include "taiji.h"
#include "SynthStream.h"
#include <iostream>

int taiji_encode(std::string in, char *out1, char *out2, char mode)
{
	SynthStream synth(out1, out2, mode);
	synth.write(in.c_str(), in.length());
	synth.flush();
	synth.close();
	return 0;	//no error check for now...
}

int taiji_decode(char *in1, char *in2, std::string &out, char mode)
{
	SynthStream synth(in1, in2, mode);
	while (!synth.eof())
	{
		out += synth.get();
	}
	synth.close();
	return 0;	//no error check for now...
}

int taiji_encode_to_files(char *in, char *out1, char *out2, char mode)
{
	std::fstream fs;
	std::string data;

	fs.open(in, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	if (!fs.is_open())
	{
		fs.open(in, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	}
	while (fs.good())
	{
		data += fs.get();
	}

	int error = taiji_encode(data, out1, out2, mode);

	return error;
}

int taiji_decode_from_files(char *in1, char *in2, char *out, char mode)
{
	std::fstream fs;

	std::string data;

	fs.open(out, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	if (!fs.is_open())
	{
		fs.open(out, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	}

	int error = taiji_decode(in1, in2, data, mode);

	if (error == 0)	//only write to file if completed successfully.
	{
		//printf("Number of characters found: %d\n", data.length());
		//printf(data.c_str());	//prints to screen.
		fs.write(data.c_str(), data.length());
	}
	return error;
}