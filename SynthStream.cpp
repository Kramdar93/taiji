/////////////////////////
// (c) 2015 Mark Elsinger
// SynthStream implementation
// Splits bytes in half and distributes between 2 files.
// Intended to be used with encrypted data, 
// does not hide patterns well enough alone.
// In fact, don't even use with any encryption
// method that preserves patterns like 
// anything in a block coding method.
/////////////////////////

#include "SynthStream.h"
#include <fstream>
#include <iostream>

//#define DEBUG	//toggles debug output
#define WINDOWS	//for compiling with windows

/*	Constructor for a SynthStream object without associting with a file.
	char m defines the spliting mode
		'h' for half&half, first half of each byte in one file, rest in another
		'i' for interleaved, every other bit in one file, rest in the other.
*/
SynthStream::SynthStream(char m)
{
	hasOverflow = false;
	mode = m;
	if (mode != 'h' && mode != 'i')	//check mode input
	{
		mode = 'h';
#ifdef DEBUG
		printf("Undefined mode given to SynthStream, defaulting to half-n-half.\n");
#endif
	}
}

/*	Constructor for a SynthStream object associated with two files.
	char m defines the spliting mode
		'h' for half&half, first half of each byte in one file, rest in another
		'i' for interleaved, every other bit in one file, rest in the other.
	char *c1 & *c2 are names of the files to use.
*/
SynthStream::SynthStream(char* c1, char* c2, char m)
{
	open(c1, c2);
	hasOverflow = false;
	mode = m;
	if (mode != 'h' && mode != 'i')	//check mode input
	{
		mode = 'h';
#ifdef DEBUG
		printf("Undefined mode given to SynthStream, defaulting to half-n-half.\n");
#endif
	}
}

/*	Constructor for a SynthStream object associated with two files.
	char m defines the spliting mode
		'h' for half&half, first half of each byte in one file, rest in another
		'i' for interleaved, every other bit in one file, rest in the other.
	string s1 and s2 are names of the files to use.
*/
SynthStream::SynthStream(std::string& s1, std::string& s2, char m)
{
	open(s1, s2);
	hasOverflow = false;
	mode = m;
	if (mode != 'h' && mode != 'i') //check mode input
	{
		mode = 'h';
#ifdef DEBUG
		printf("Undefined mode given to SynthStream, defaulting to half-n-half.\n");
#endif
	}
}

/*	Opens two files to use. Make sure the order of them is consistant.
*/
void SynthStream::open(char* c1, char* c2)
{
	//open each in binary i/o
	f1.open(c1, ios_base::in | ios_base::out | ios_base::binary);
	f2.open(c2, ios_base::in | ios_base::out | ios_base::binary);

	//check if they opened, attempt opening with truncation otherwise.
	if (!f1.is_open())
	{
		f1.open(c1, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);
	}
	if (!f2.is_open())
	{
		f2.open(c2, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);
	}
	hasOverflow = false;
}

/*	Opens two files to use. Make sure the order of them is consistant.
*/
void SynthStream::open(std::string& s1, std::string& s2)
{
	//open each in binary i/o
	f1.open(s1);
	f2.open(s2);

	//check if they opened, attempt opening with truncation otherwise.
	if (!f1.is_open())
	{
		f1.open(s1, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);
	}
	if (!f2.is_open())
	{
		f2.open(s2, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);
	}
	hasOverflow = false;
}

/*	Returns if the stream is open, if either stream is not then it can't be either.
*/
bool SynthStream::is_open()
{
	return f1.is_open() && f2.is_open();
}

/*	Closes stream and file substreams.
	Should probably flush the stream first to make sure any overflow is written.
*/
void SynthStream::close()
{
	f1.close();
	f2.close();
}

/*  Get single character.
	Only every other call to this will retrieve a character from the actual files!
*/
char SynthStream::get()
{
	char res;	//return character
	if (mode == 'h')	//Half&Half
	{
		if (hasOverflow)
		{
			// already have some leftover characters so use them
			hasOverflow = false;
			res = (overflow1 << 4) | overflow2;
#ifdef DEBUG
			printf("Got overflow: %c\n", res);
#endif
		}
		else //need to read more in
		{
			hasOverflow = true;
			char c1 = f1.get();
			char c2 = f2.get();
			overflow1 = (c1 & 0xF0) >> 4;	//do it in reverse for fun i guess.
			overflow2 = (c2 & 0xF0) >> 4;
			res = (c1 & 0x0F) << 4 | (c2 & 0x0F);
#ifdef DEBUG
			printf("Got char: %c\n", res);
#endif
		}
	}
	else	//if (mode == 'i') 
	{
		if (hasOverflow)
		{
			//use overflow bits
			hasOverflow = false;
			recompose(overflow1, overflow2, &res);
#ifdef DEBUG
			printf("Got overflow: %c\n", res);
#endif
		}
		else
		{
			hasOverflow = true;
			char c1 = f1.get();
			char c2 = f2.get();
			overflow1 = (c1 & 0xF);	//store overflow
			overflow2 = (c2 & 0xF);
			recompose(c1 >> 4, c2 >> 4, &res);
#ifdef DEBUG
			printf("Got char: %c\n", res);
#endif
		}
	}

#ifdef WINDOWS
	if (eof() && !fail() && res != 26)  //don't quit unless you meant to (Windows)
	{
		f1.clear();
		f2.clear();
	}
#endif
	return res;
}

/*  Get single character.
	Only every other call to this will retrieve a character from the actual files!
	char &c reference to char where return value is to be written.
*/
void SynthStream::get(char& c)
{
	char res = get();
	c = res;
}

/*  Get entire line or until n characters are read.
	char *c is pointer to beginning of cstring to write to.
*/
void SynthStream::getline(char* c, std::streamsize n)
{
	for (std::streamsize i = 0; i < n; ++i)
	{
		c[i] = get();
		if (c[i] == '\n')
		{
			break;
		}
	}
}

/*  Retrieve a single character from the stream and discard it.
*/
void SynthStream::ignore()
{
	get();
}

/*	Returns the next character without removing it from the stream.
	(Actually does, just puts it back...)
*/
char SynthStream::peek()
{
	char p = get();
	put(p);
	return p;
}

/*	Returns the position of the stream in the file.
	Compile a debug version to enable error reporting if there is a problem
	to check that the position is the same in each substream.
*/
std::streampos SynthStream::tellg()
{
	std::streampos s = f1.tellg();
#ifdef DEBUG
	if (s != f2.tellg())
	{
		std::cerr << "Locations do not match up.";
	}
#endif
	return hasOverflow ? s * 2 + 1 : s * 2;	//return twice the substreampos, +1 if there is overflow.
}

/*	Set the position in the stream.
*/
void SynthStream::seekg(streampos pos)
{
	std::cerr << "Not implemented yet.";
}

/*	put a single character in.
	only actually writes to files every other call!
	make sure to flush when done to make sure it all writes.
*/
void SynthStream::put(char c)
{
	if (mode == 'h')
	{
		if (hasOverflow)
		{
			hasOverflow = false;
			char c1 = c & 0xF0 | overflow1;
			char c2 = ((c & 0x0F) << 4) | overflow2;
			f1.put(c1);
			f2.put(c2);
#ifdef DEBUG
			printf("Put char: %c\n", c);
#endif
		}
		else
		{
			hasOverflow = true;
			overflow1 = (c & 0xF0) >> 4;
			overflow2 = c & 0x0F;
#ifdef DEBUG
			printf("Overflow: %c\n", (overflow1 << 4) | overflow2);
#endif
		}
	}
	else //if(mode == 'i')
	{
		if (hasOverflow)
		{
			hasOverflow = false;
			char c1, c2;
			decompose(c, &c1, &c2);
			c1 = (overflow1 << 4) | c1;
			c2 = (overflow2 << 4) | c2;
			f1.put(c1);
			f2.put(c2);
#ifdef DEBUG
			printf("Put char: %c\n", c);
#endif
		}
		else
		{
			hasOverflow = true;
			char c1, c2;
			decompose(c, &c1, &c2);
			overflow1 = c1;
			overflow2 = c2;
#ifdef DEBUG
			printf("Overflow: %c\n", c);
#endif
		}
	}
}

/*	Put a cstring in.
	Still may have an unwritten character
*/
void SynthStream::write(const char* c, std::streamsize n)
{
	for (std::streamsize i = 0; i < n; ++i)
	{
		put(c[i]);
	}
}

//same as tellg
std::streampos SynthStream::tellp()
{
	std::streampos s = f1.tellp();
#ifdef DEBUG
	if (s != f2.tellp())
	{
		std::cerr << "Locations do not match up.";
	}
#endif
	return hasOverflow ? s * 2 + 1 : s * 2;	//return twice the substreampos, +1 if there is overflow.
}

//same as seekg
void SynthStream::seekp(streampos pos)
{
	std::cerr << "Not implemented yet.";
}

//	Returns if both files are good
bool SynthStream::good()
{
	return f1.good() && f2.good();
}

//	Returns if either file is bad
bool SynthStream::bad()
{
	return f1.bad() || f2.bad();
}

//	Returns if either eof is reached.
bool SynthStream::eof()
{
	return f1.eof() || f2.eof();
}

//	Returns if either file's fail bit is set.
bool SynthStream::fail()
{
	return f1.fail() || f2.fail();
}

//writes any leftover characters, only use when done with writing.
void SynthStream::flush()
{
	if (hasOverflow)
	{
		put(0x0);
	}
	f1.flush();
	f2.flush();
}

/*	Takes a character and splits it into two separate characters, with leading half of byte to be ignored.
	o1 is the odd bits.
	o2 is the other bits.
	used in interleave mode
*/
void SynthStream::decompose(char in, char* o1, char* o2)
{
	int charsize = 8 * sizeof(char);	//size of char in bits (assumes byte is 8 bits)
	int i;

	*o1 = *o2 = 0x0;
#ifdef DEBUG
	char in_o = in;
	printf("charsize: %d\n", charsize);
#endif
	for (i = 0; i < charsize-2; ++i)
	{
		if ((i % 2) == 0)	//every other step
		{
			*o1 = *o1 | ( (in >> (charsize - 1)) & 0x1);	//add first bit to end.
			*o1 = *o1 << 1;	//shift left once.
		}
		else
		{
			*o2 = *o2 | ( (in >> (charsize - 1)) & 0x1);	//add first bit to end.
			*o2 = *o2 << 1;	//shift left once.
		}
		in = in << 1;	//shift the input left to get next 'first' bit
	}
	//do last step w/o shifts
	if ((i % 2) == 0)	//every other step
	{
		*o1 = *o1 | ((in >> (charsize - 1)) & 0x1);	//add first bit to end.
		in = in << 1;
		*o2 = *o2 | ((in >> (charsize - 1)) & 0x1);	//add first bit to end.
	}
	else
	{
		*o2 = *o2 | ((in >> (charsize - 1)) & 0x1);	//add first bit to end.
		in = in << 1;
		*o1 = *o1 | ((in >> (charsize - 1)) & 0x1);	//add first bit to end.
	}
#ifdef DEBUG
	/*printf("mask (reverse order): ");
	for (i = 0; i < charsize; ++i)
	{
		printf("%i", mask & 0x1);
		mask = mask >> 1;
	}*/
	char o1_o = *o1;
	char o2_o = *o2;
	printf("input:           ");
	for (i = 0; i < charsize; ++i)
	{
		printf("%i", (in_o >> (charsize-1)) & 0x1);
		in_o = in_o << 1;
	}
	printf("\noutputs: ");
	for (i = 0; i < charsize; ++i)
	{
		printf("%i ", (o1_o >> (charsize - 1)) & 0x1);
		o1_o = o1_o << 1;
	}
	printf("\n         ");
	for (i = 0; i < charsize; ++i)
	{
		printf(" %i", (o2_o >> (charsize - 1)) & 0x1);
		o2_o = o2_o << 1;
	}
	printf("\n");
#endif
}

/*	takes two characters and recombines them into the initial character
	(reverse of decompose)
	First half of input bytes are ignored.
	used for interleave mode
*/
void SynthStream::recompose(char in1, char in2, char* o)
{
	int charsize = 8 * sizeof(char);	//size of char in bits (assumes byte is 8 bits)
	int i;

	*o = 0x0;
	in1 = in1 << (charsize / 2);
	in2 = in2 << (charsize / 2);
#ifdef DEBUG
	char in1_o = in1;
	char in2_o = in2;
#endif
	for (i = 0; i < charsize-2; ++i)
	{
		if ((i % 2) == 0)	//every other step
		{
			*o = *o | 
				((in1 >> (charsize - 1)) & 0x1);	//add first bit to end.
			in1 = in1 << 1;	//shift the input to the left
		}
		else
		{
			*o = *o |
				((in2 >> (charsize - 1)) & 0x1);	//add first bit to end.
			in2 = in2 << 1;	//shift to the left
		}
		*o = *o << 1;	//shift left once to make room
	}
	if ((i % 2) == 0)	//every other step
	{
		*o = *o |
			((in1 >> (charsize - 1)) & 0x1);	//add first bit to end.
		*o = *o << 1;	//shift left once.
		*o = *o |
			((in2 >> (charsize - 1)) & 0x1);	//add first bit to end.
	}
	else
	{
		*o = *o |
			((in2 >> (charsize - 1)) & 0x1);	//add first bit to end.
		*o = *o << 1;	//shift left once.
		*o = *o |
			((in1 >> (charsize - 1)) & 0x1);	//add first bit to end.
	}
#ifdef DEBUG
	/*printf("mask (reverse order): ");
	for (i = 0; i < charsize; ++i)
	{
		printf("%i", mask & 0x1);
		mask = mask >> 1;
	}*/
	char o_o = *o;
	printf("inputs: ");
	for (i = 0; i < charsize; ++i)
	{
		printf("%i ", (in1_o >> (charsize - 1)) & 0x1);
		in1_o = in1_o << 1;
	}
	printf("\n        ");
	for (i = 0; i < charsize; ++i)
	{
		printf(" %i", (in2_o >> (charsize - 1))& 0x1);
		in2_o = in2_o << 1;
	}
	printf("\noutput: ");
	for (i = 0; i < charsize; ++i)
	{
		printf("%i", (o_o >> (charsize - 1)) & 0x1);
		o_o = o_o << 1;
	}
	printf("\n");
#endif
}