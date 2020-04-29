#ifndef IO_H
#define IO_H
#include <string>
#include <stdexcept>

using namespace std;

void read_cli_args(int argc, char* argv[], string& in, string& out, string& operation, float& scale) 
{
	if (argc < 5)
	{
		string prog_name = argv[0];
		throw runtime_error("Usage: " + prog_name + " INPUT_FILE OUTPUT_FILE OPERATION[enlarge,resize] SCALE");
	}

	in = argv[1];
	out = argv[2];
	operation = argv[3];

	if (operation == "enlarge") 
	{
		int n = atoi(argv[4]);

		if (n < 1)
		{
			throw runtime_error("Scale must be a positive integer minimum 1");
		}
		
		scale = n;
	}
	else if (operation == "resize") 
	{
		scale = atof(argv[4]);

		if (scale < 0)
		{
			throw runtime_error("Scale must be a positive float number");
		}
	}
	else {
		throw runtime_error("Unknown operation. Use one of: enlarge, resize");
	}
}

#endif