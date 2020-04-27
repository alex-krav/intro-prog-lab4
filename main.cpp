#include "bmp.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        cerr << "Usage: " << argv[0] << " NUMBER INPUT_FILE OUTPUT_FILE" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    const char* infile = argv[2];
    const char* outfile = argv[3];

    std::ifstream inptr{ infile, std::ios_base::binary };
    if (!inptr)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    std::ofstream outptr{ outfile, std::ios_base::binary };
    if (!outptr) 
    {
        inptr.close();
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    
    BMPHEAD1 b1;
    BMPHEAD2 b2;
    inptr.read((char*)&b1, sizeof(b1));
    inptr.read((char*)&b2, sizeof(b2));

    if ((b1.id[0] != 'B' && b1.id[1] != 'M') || b2.headersize != 54 || b2.infoSize != 40 ||
        b2.bits != 24 || b2.biCompression != 0)
    {
        outptr.close();
        inptr.close();
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    int old_padding = (4 - (b2.width * sizeof(PIXELDATA)) % 4) % 4;

    int img_height = b2.depth;
    int img_width = b2.width;

    b2.depth *= n;
    b2.width *= n;
    int resized_height = b2.depth;
    int resized_width = b2.width;

    int new_padding = (4 - (resized_width * sizeof(PIXELDATA)) % 4) % 4;

    b2.biSizeImage = ((resized_width * sizeof(PIXELDATA) + new_padding) * abs(resized_height));
    b2.filesize = b2.biSizeImage + (sizeof(BMPHEAD1) + sizeof(BMPHEAD2));

    outptr.write((const char*)&b1, sizeof(b1));
    outptr.write((const char*)&b2, sizeof(b2));

    for (int i = 0; i < img_height; ++i)
    {
        for (int q = 0; q < n; ++q)
        {
            for (int j = 0; j < img_width; ++j)
            {
                PIXELDATA pixel;

                inptr.read((char*)&pixel, sizeof(pixel));

                for (int r = 0; r < n; ++r)
                    outptr.write((const char*)&pixel, sizeof(pixel));
            }

            inptr.seekg(old_padding, inptr.cur);

            for (int k = 0; k < new_padding; ++k)
                outptr.put(0x00);

            if (q < n - 1)
                inptr.seekg(-(img_width*3 + old_padding), inptr.cur);
        }
    }
    
    inptr.close();
    outptr.close();
	
	return 0;
}