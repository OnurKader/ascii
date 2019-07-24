#include </usr/include/c++/7/iostream>
#include <sys/ioctl.h>
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

struct vec2
{
	unsigned int x;
	unsigned int y;
};

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

	// ASCII Character list of 67 characters ranging from 0 Brightness to 255
	const char symbols[] = " .`^\",:;Il!i~+_-?][}{1)(|/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

	CImg<unsigned char> image(image_path.c_str());

	vec2 image_size = {image._width, image._height};

	// If Image's Aspect Ratio is greater than 1 work with Width, else work with Height;
	float image_aspect = (float)image_size.x / (float)image_size.y;
	float term_aspect = (float)term_width / (float)term_height;

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
