#include "bmp.h"
#include <stdexcept>
#include <iostream>

BMP::BMP(const string fname) {
	read(fname);
}

void BMP::check_file_format(BMPHEAD1 b1, BMPHEAD2 b2)
{
	if ((b1.id != 0x4D42) || b2.headersize != 54 || b2.infoSize != 40 ||
		b2.bits != 24 || b2.biCompression != 0)
	{
		input.close();
		throw runtime_error("Unsupported file format.");
	}
}

void BMP::resize_vec(vector<vector<PIXELDATA>>& vec, const unsigned short ROWS, const unsigned short COLUMNS)
{
	vec.resize(ROWS);
	for (auto& it : vec)
	{
		it.resize(COLUMNS);
	}
}

void BMP::read(const string fname) 
{
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

void BMP::resize(string operation, float scale) 
{
	if (operation == "enlarge")
		enlarge(scale);
	else if (operation == "resize")
		resize(scale);
	else 
		throw runtime_error("Unknown operation. Use one of: enlarge, resize");
}

void BMP::enlarge(int n) 
{
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
					int row = i * n + q;
					int col = j * n + r;
					resized_data[row][col] = data[i][j];
				}

	data = resized_data;
	cout << "Done." << endl;
}

void BMP::write(const string fname) 
{
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

/******************** Bilinear interpolation **********************/

float BMP::lerp(float s, float e, float t)
{ 
	return s + (e - s) * t; 
}

float BMP::blerp(float c00, float c10, float c01, float c11, float tx, float ty)
{
	return BMP::lerp(BMP::lerp(c00, c10, tx), BMP::lerp(c01, c11, tx), ty);
}

uint32_t BMP::getpixel(vector<vector<PIXELDATA>>& data, unsigned int x, unsigned int y) {
	return data[y][x].get();
}

void BMP::putpixel(vector<vector<PIXELDATA>>& data, unsigned int x, unsigned int y, uint32_t color) {
	data[y][x].set(color);
}

void BMP::resize(float n)
{
	cout << "Resizing image " << n << " times... ";

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

	for (int y = 0; y < resized_height; ++y) {
		for (int x = 0; x < resized_width; ++x) {
			float gx = ((float)x) / resized_width * (img_width - 1);
			float gy = ((float)y) / resized_height * (img_height - 1);
			int gxi = (int)gx;
			int gyi = (int)gy;
			uint32_t rgb = 0;
			uint32_t c00 = getpixel(data, gxi, gyi);
			uint32_t c10 = getpixel(data, gxi + 1, gyi);
			uint32_t c01 = getpixel(data, gxi, gyi + 1);
			uint32_t c11 = getpixel(data, gxi + 1, gyi + 1);
			for (int i = 0; i < PIXEL_SIZE; ++i) {
				uint8_t b00 = getByte(c00, i);
				uint8_t b10 = getByte(c10, i);
				uint8_t b01 = getByte(c01, i);
				uint8_t b11 = getByte(c11, i);
				uint32_t ble = ((uint8_t)blerp(b00, b10, b01, b11, gx - gxi, gy - gyi)) << (8 * i);
				rgb |= ble;
			}
			putpixel(resized_data, x, y, rgb);
		}
	}

	data = resized_data;
	cout << "Done." << endl;
}
