#ifndef BMP_H
#define BMP_H
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

using namespace std;

// https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/
struct BMPHEAD1 {
	int16_t id;
};
struct BMPHEAD2 {
	int32_t filesize;
	int16_t reserved[2];
	int32_t headersize;

	int32_t infoSize;
	int32_t width;
	int32_t depth;
	int16_t biPlanes;
	int16_t bits;
	int32_t biCompression;
	int32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	int32_t biClrUsed;
	int32_t biClrImportant;
};

struct PIXELDATA {
	int8_t blueComponent;
	int8_t greenComponent;
	int8_t redComponent;
};

class BMP {
	static const int BMPHEAD1_SIZE = sizeof(BMPHEAD1);
	static const int BMPHEAD2_SIZE = sizeof(BMPHEAD2);
	static const int PIXEL_SIZE = sizeof(PIXELDATA);

	BMPHEAD1 bmp_id_header;
	BMPHEAD2 bmp_info_header;
	vector<vector<PIXELDATA>> data;
	ifstream input;
	ofstream output;

	void check_file_format(BMPHEAD1 b1, BMPHEAD2 b2) 
	{
		if ((b1.id != 0x4D42) || b2.headersize != 54 || b2.infoSize != 40 ||
			b2.bits != 24 || b2.biCompression != 0)
		{
			input.close();
			throw runtime_error("Unsupported file format.");
		}
	}

	void resize_vec(vector<vector<PIXELDATA>>& vec, const unsigned short ROWS, const unsigned short COLUMNS)
	{
		vec.resize(ROWS);
		for (auto& it : vec)
		{
			it.resize(COLUMNS);
		}
	}

public:
	BMP(const string fname) {
		read(fname);
	}

	void read(const string fname) {
		input.open(fname, ios_base::binary);
		if (!input)
		{
			throw runtime_error("Could not open " + fname);
		}

		input.read((char*)&bmp_id_header, BMPHEAD1_SIZE);
		input.read((char*)&bmp_info_header, BMPHEAD2_SIZE);

		check_file_format(bmp_id_header, bmp_info_header);

		int old_padding = (4 - (bmp_info_header.width * PIXEL_SIZE) % 4) % 4;
		int img_height = bmp_info_header.depth;
		int img_width = bmp_info_header.width;

		resize_vec(data, img_height, img_width);

		for (int i = 0; i < img_height; ++i)
		{
			for (int j = 0; j < img_width; ++j)
			{
				PIXELDATA pixel;

				input.read((char*)&pixel, PIXEL_SIZE);

				data[i][j] = pixel;
			}

			input.seekg(old_padding, input.cur);
		}
		input.close();
	}

	void resize(int n) {
		cout << "Enlarging image " << n << " times... ";

		int img_height = bmp_info_header.depth;
		int img_width = bmp_info_header.width;

		bmp_info_header.depth *= n;
		bmp_info_header.width *= n;
		int resized_height = bmp_info_header.depth;
		int resized_width = bmp_info_header.width;

		int new_padding = (4 - (resized_width * PIXEL_SIZE) % 4) % 4;
		bmp_info_header.biSizeImage = (resized_width * PIXEL_SIZE + new_padding) * resized_height;
		bmp_info_header.filesize = bmp_info_header.biSizeImage + BMPHEAD1_SIZE + BMPHEAD2_SIZE;

		vector<vector<PIXELDATA>> resized_data;
		resize_vec(resized_data, resized_height, resized_width);

		for (int i = 0; i < img_height; ++i)
			for (int q = 0; q < n; ++q)
				for (int j = 0; j < img_width; ++j)
					for (int r = 0; r < n; ++r) {
						int row = i*n + q;
						int col = j*n + r;
						resized_data[row][col] = data[i][j];
					}

		data = resized_data;
		cout << "Done." << endl;
	}

	void write(const string fname) {
		output.open(fname, ios_base::binary);
		if (!output)
		{
			throw runtime_error("Could not create " + fname);
		}

		output.write((const char*)&bmp_id_header, sizeof(bmp_id_header));
		output.write((const char*)&bmp_info_header, sizeof(bmp_info_header));

		int padding = (4 - (bmp_info_header.width * PIXEL_SIZE) % 4) % 4;
		int img_height = bmp_info_header.depth;
		int img_width = bmp_info_header.width;

		for (int i = 0; i < img_height; ++i)
		{
			for (int j = 0; j < img_width; ++j)
				output.write((const char*)&data[i][j], PIXEL_SIZE);

			for (int k = 0; k < padding; ++k)
				output.put(0x00);
		}

		output.close();
		cout << "Written result to " << fname << endl;
	}
};

#endif