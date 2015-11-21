///////////////////////////
// (c) 2015 Mark Elsinger
// taiji
// Function definitions for easier implementation of SynthStream.
///////////////////////////

#ifndef __TAIJI_H__
#define __TAIJI_H__

#include <string>

//encode string in to files out1 & out2 given mode. Returns 0 on success.
int taiji_encode(std::string in, char *out1, char *out2, char mode);

//decode string out from files in1 & in2 given mode. Returns 0 on success.
int taiji_decode(char *in1, char *in2, std::string &out, char mode);

//encode file in to files out1 & out2 given mode. Returns 0 on success.
int taiji_encode_to_files(char *in, char *out1, char *out2, char mode);

//decode file out from files in1 & in2 given mode. Returns 0 on success.
int taiji_decode_from_files(char *in1, char *in2, char *out, char mode);

#endif