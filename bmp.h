#ifndef BMP_H
#define BMP_H
#include <vector>
#include <string>
#include <fstream>

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
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	uint32_t get() {
		uint32_t rgb = red;
		rgb = (rgb << 8) + green;
		rgb = (rgb << 8) + blue;
		return rgb;
	}

	void set(uint32_t rgb) {
		red = (rgb >> 16) & 0xFF;
		green = (rgb >> 8) & 0XFF;
		blue = rgb & 0xFF;
	}
};

class BMP {
	static const int BMPHEAD1_SIZE = sizeof(BMPHEAD1);
	static const int BMPHEAD2_SIZE = sizeof(BMPHEAD2);
	static const int PIXEL_SIZE = sizeof(PIXELDATA);

	static uint8_t getByte(uint32_t value, int n) {
		return (value >> (n * 8)) & 0xFF;
	}

	BMPHEAD1 bmp_id_header;
	BMPHEAD2 bmp_info_header;
	vector<vector<PIXELDATA>> data;
	ifstream input;
	ofstream output;

	void check_file_format(BMPHEAD1 b1, BMPHEAD2 b2);
	void resize_vec(vector<vector<PIXELDATA>>& vec, const unsigned short ROWS, const unsigned short COLUMNS);
	float lerp(float s, float e, float t);
	float blerp(float c00, float c10, float c01, float c11, float tx, float ty);
	uint32_t getpixel(vector<vector<PIXELDATA>>& data, unsigned int x, unsigned int y);
	void putpixel(vector<vector<PIXELDATA>>& data, unsigned int x, unsigned int y, uint32_t color);

public:
	BMP(const string fname);
	~BMP() {}

	void read(const string fname);
	void enlarge(int n);
	void resize(float n);
	void resize(string operation, float scale);
	void write(const string fname);
};

#endif