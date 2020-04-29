#include "bmp.h"
#include "io.h"
#include <iostream>

int main(int argc, char* argv[])
{
    try {
        string infile;
        string outfile;
        string operation;
        float scale;
        read_cli_args(argc, argv, infile, outfile, operation, scale);

        BMP bmp(infile);
        bmp.resize(operation, scale);
        bmp.write(outfile);
    }
    catch (exception e) {
        cerr << e.what() << endl;
        return 1;
    }
	
	return 0;
}