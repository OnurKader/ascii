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
unsigned int brightness(const unsigned char &red, const unsigned char &green, const unsigned char &blue);

/// MAIN FUNCTION
int main(int argc, char **argv)
{
	std::ios_base::sync_with_stdio(false);
	// Setup The Arguments
	Args arg_parser(argc, argv);
	arg_parser.convert();

	// Find Terminal Size
	unsigned short term_width, term_height;
	getSize(term_width, term_height);

	std::string image_path;

	for (auto &item : arg_parser.getOpts())
		if (item.second.mode == Option::str && item.second.name != argv[0U])
			image_path = item.second.name;
	bool colorful = arg_parser.optExists("-c") || arg_parser.optExists("--color");
	bool inverse = arg_parser.optExists("-i") || arg_parser.optExists("--inverse");

	// ASCII Character list of 70 characters ranging from 0 Brightness to 255
	/* const char symbols[] = "  .`\'^\",:;Il!i~+_-?][}{1)(|/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#&$8%BN@MW"; */
	// 102 chars
	const char symbols[] = "  ..,,,'''``--_:;^^**""=+<>iv%&xclrs)/){}I?!][1taeo7zjLunT#@JCwfy325Fp6mqSghVd4EgXPGZbYkOA8U$KHDBWNMR0Q";

	CImg<unsigned char> image(image_path.c_str());
	const bool grey_scale = image.spectrum() == 1U;

	const unsigned int new_height = term_height * 8U;
	const unsigned int new_width = term_width * 4U;
	const float image_aspect = (float)image._width / (float)image._height;

	if(image_aspect < 1.f)
		image.resize(new_height * image_aspect, new_height);
	else
		image.resize(new_width, new_width/image_aspect);

	for(unsigned int rows = 0U; rows <= image.height() - 8; rows += 8)
	{
		for(unsigned int cols = 0U; cols <= image.width() - 4; cols += 4)
		{
			if(grey_scale || !colorful)
			{
				unsigned int b = image._atXY(cols, rows, 0, 0);
				std::cout << symbols[map(b, 0U, 256U, 0U, 101U)];
			}
			else {
				std::cout << "\033[38;2;0;0;0m";
				uint8_t r = image._atXY(cols, rows, 0, 0);
				uint8_t g = image._atXY(cols, rows, 0, 1);
				uint8_t b = image._atXY(cols, rows, 0, 2);
				std::cout << (inverse ? "\033[48;2;" : "\033[38;2;") << (uint16_t)r << ';' << (uint16_t)g << ';' << (uint16_t)b << 'm' << symbols[map(brightness(r, g, b), 0U, 256U, 0U, 101U)];
			}
		}
		std::cout << "\033[m" << std::endl;
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

unsigned int brightness(const unsigned char &red, const unsigned char &green, const unsigned char &blue)
{
	return (unsigned int)(red * 0.22f + green * 0.71f + blue * 0.07f);
}


