///////////////////////////
// (c) 2015 Mark Elsinger
// SynthString
// Reads and writes to a split file.
// Designed for security via obsfucation.
// NOTE: not all methods of fstream are implemented so 
// adjust usage accordingly.
///////////////////////////


#ifndef __SYNTHSTREAM__
#define __SYNTHSTREAM__

#include <fstream>

class SynthStream : private std::fstream
{
public:
	//constructors
	SynthStream(char m);
	SynthStream(char* c1, char* c2, char m);
	SynthStream(std::string& s1, std::string& s2, char m);

	//file operations
	void open(char* c1, char* c2);
	void open(std::string& s1, std::string& s2);
	void close();

	//input ops
	char get();
	void get(char& c);
	void getline(char* c, std::streamsize n);
	void ignore();
	char peek();
	streampos tellg();
	void seekg(streampos pos);

	//output ops
	void write(const char* c, std::streamsize n);
	void put(char c);
	streampos tellp();
	void seekp(streampos pos);

	//flag wrappers
	bool is_open();
	bool good();
	bool bad();
	bool eof();
	bool fail();

	void flush();

private:
	//substreams
	std::fstream f1, f2;

	//overflow characters for each stream since it only reads/writes half as often.
	char overflow1, overflow2;
	bool hasOverflow;

	//mode for dividing the data.
	char mode;

	//functions for interleaving bits.
	void decompose(char in, char* o1, char* o2);
	void recompose(char in1, char in2, char* o);
};

#endif