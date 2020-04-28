#include "bmp.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
    int BMPHEAD1_SIZE = sizeof(BMPHEAD1);
    int BMPHEAD2_SIZE = sizeof(BMPHEAD2);
    int PIXEL_SIZE = sizeof(PIXELDATA);

    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s INPUT_FILE OUTPUT_FILE NUMBER\n", argv[0]);
        return 1;
    }

    const char* infile = argv[1];
    const char* outfile = argv[2];
    int n = atoi(argv[3]);

    if (n < 1)
    {
        printf("Size number must be a positive integer minimum 1\n");
        return 2;
    }

    ifstream input{ infile, ios_base::binary };
    if (!input)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    ofstream output{ outfile, ios_base::binary };
    if (!output) 
    {
        input.close();
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }
    
    BMPHEAD1 b1;
    BMPHEAD2 b2;
    input.read((char*)&b1, BMPHEAD1_SIZE);
    input.read((char*)&b2, BMPHEAD2_SIZE);
    
    if ((b1.id != 0x4D42) || b2.headersize != 54 || b2.infoSize != 40 ||
        b2.bits != 24 || b2.biCompression != 0)
    {
        output.close();
        input.close();
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }

    printf("Enlarging image %d times... ", n);
    
    int old_padding = (4 - (b2.width * PIXEL_SIZE) % 4) % 4;

    int img_height = b2.depth;
    int img_width = b2.width;

    b2.depth *= n;
    b2.width *= n;
    int resized_height = b2.depth;
    int resized_width = b2.width;

    int new_padding = (4 - (resized_width * PIXEL_SIZE) % 4) % 4;

    b2.biSizeImage = (resized_width * PIXEL_SIZE + new_padding) * resized_height;
    b2.filesize = b2.biSizeImage + BMPHEAD1_SIZE + BMPHEAD2_SIZE;

    output.write((const char*)&b1, sizeof(b1));
    output.write((const char*)&b2, sizeof(b2));

    for (int i = 0; i < img_height; ++i)
    {
        for (int q = 0; q < n; ++q)
        {
            for (int j = 0; j < img_width; ++j)
            {
                PIXELDATA pixel;

                input.read((char*)&pixel, PIXEL_SIZE);

                for (int r = 0; r < n; ++r)
                    output.write((const char*)&pixel, PIXEL_SIZE);
            }

            input.seekg(old_padding, input.cur);

            for (int k = 0; k < new_padding; ++k)
                output.put(0x00);

            if (q < n - 1)
                input.seekg(-(img_width * PIXEL_SIZE + old_padding), input.cur);
        }
    }
    
    input.close();
    output.close();
    printf("Done.\nWritten result to %s\n", outfile);
	
	return 0;
}