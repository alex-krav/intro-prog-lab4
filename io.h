#ifndef IO_H
#define IO_H
#include <string>
#include <stdexcept>

using namespace std;

void read_cli_args(int argc, char* argv[], string& in, string& out, int& n) 
{
	if (argc < 4)
	{
		string prog_name = argv[0];
		throw runtime_error("Usage: " + prog_name + " INPUT_FILE OUTPUT_FILE NUMBER");
	}

	n = atoi(argv[3]);

	if (n < 1)
	{
		throw runtime_error("Size number must be a positive integer minimum 1");
	}

	in = argv[1];
	out = argv[2];
}

#endif