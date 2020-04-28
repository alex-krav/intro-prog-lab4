#include "bmp.h"
#include "io.h"

int main(int argc, char* argv[])
{
    try {
        string infile;
        string outfile;
        int n;
        read_cli_args(argc, argv, infile, outfile, n);

        BMP bmp(infile);
        bmp.resize(n);
        bmp.write(outfile);
    }
    catch (exception e) {
        cerr << e.what() << endl;
        return 1;
    }
	
	return 0;
}