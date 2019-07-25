#include </usr/include/c++/7/iostream>
#include <sys/ioctl.h>
#define cimg_display 0
#include <CImg/CImg.h>
#include "args.hpp"

using namespace cimg_library;

// Get a Value in an initial range (domain) and convert it into another range
// eg. sin(x) : [-1, 1] -> [3, 7]
template <typename T>
T map(T current_value, T minimum_domain_value, T maximum_domain_value, T minimum_range_value, T maximum_range_value);

// Get the Terminal size using ioctl and store it in these variables
// Fullscreen on my machine is (191x52)
void getSize(unsigned short &width, unsigned short &height);

// Get the Luminesence, human eyes see green better, use a formula from the interwebs
int brightness(unsigned char &red, unsigned char &green, unsigned char &blue);

/// MAIN FUNCTION
int main(int argc, char **argv)
{
	// Setup The Arguments
	Args arg_parser(argc, argv);
	arg_parser.convert();

	// Find Terminal Size
	unsigned short term_width, term_height;
	getSize(term_width, term_height);

	std::string image_path;

	for (auto &item : arg_parser.getOpts())
		if (item.second.mode == Option::str && item.second.name != argv[0U])
		{
			image_path = item.second.name;
			break;
		}

	// ASCII Character list of 70 characters ranging from 0 Brightness to 255
	const char symbols[] = "  .`\'^\",:;Il!i~+_-?][}{1)(|/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#&$8%BN@MW";

	CImg<unsigned char> image(image_path.c_str());
	const bool grey_scale = image.spectrum() == 1U;

	const unsigned int new_height = term_height * 8U;
	const float image_aspect = (float)image._width / (float)image._height;
	image.resize(new_height * image_aspect, new_height, -100, -100, 5);
	for(unsigned int rows = 0U; rows <= image.height() - 8; rows += 8)
	{
		for(unsigned int cols = 0U; cols <= image.width() - 4; cols += 4)
		{
			unsigned char r = image._atXY(cols, rows, 0, grey_scale ? 0 : 0);
			unsigned char g = image._atXY(cols, rows, 0, grey_scale ? 0 : 1);
			unsigned char b = image._atXY(cols, rows, 0, grey_scale ? 0 : 2);
			std::cout << "\033[38;2;" << (unsigned int)r << ';' << (unsigned int)g << ';' << (unsigned int)b << 'm' << symbols[map(brightness(r, g, b), 0, 255, 0, 69)] << "\033[m";
		}
		std::cout << std::endl;
	}
	return 0;
} /// END OF MAIN

// Functions

	template <typename T>
T map(T val, T min_dom, T max_dom, T min_ran, T max_ran)
{
	return ((val - min_dom) * (max_ran - min_ran) / (max_dom - min_dom) + min_ran);
}

void getSize(unsigned short &width, unsigned short &height)
{
	struct winsize size;
	ioctl(1, TIOCGWINSZ, &size);
	width = size.ws_col;
	height = size.ws_row;
}

int brightness(unsigned char &red, unsigned char &green, unsigned char &blue)
{
	return (int)(red * 0.22f + green * 0.71f + blue * 0.07f);
}


