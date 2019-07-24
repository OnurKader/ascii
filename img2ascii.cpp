#include </usr/include/c++/7/iostream>
#include <sys/ioctl.h>
#include "args.hpp"
#include <SFML/Graphics/Image.hpp>

/// PROTOTYPES

// Get a Value in an initial range (domain) and convert it into another range
// eg. sin(x) : [-1, 1] -> [3, 7]
template <typename T>
T map(T current_value, T minimum_domain_value, T maximum_domain_value, T minimum_range_value, T maximum_range_value);

// Get the Terminal size using ioctl and store it in these variables
// Fullscreen on my machine is (191x52)
void getSize(unsigned short &width, unsigned short &height);

// Get Luminance (Not Brightness, just for simplicity) from SFML's Color
uint8_t getBright(const sf::Color &);

// Get Luminance (Not Brightness, just for simplicity) from 3 RGB values, no Alpha
uint8_t getBright(const uint8_t Red, const uint8_t Green, const uint8_t Blue);

// Gets the average Luminance in a Cell of given dimensions
uint8_t getAverageBright(const sf::Image &image, unsigned long start_index, const unsigned short &off_x, const unsigned short &off_y);

/// MAIN FUNCTION
int main(int argc, char **argv)
{
	// Setup The Arguments
	Args arg_parser(argc, argv);
	arg_parser.convert();

	// Find Terminal Size
	unsigned short term_width, term_height;
	getSize(term_width, term_height);

	// // Make it visible and decrement by One
	// --term_width;
	// --term_height;

	std::string image_path;

	for (auto &item : arg_parser.getOpts())
		if (item.second.mode == Option::str && item.second.name != argv[0u])
		{
			image_path = item.second.name;
			break;
		}

	// TODO Scale the Image by either using a texture or by writing your own function
	sf::Image image;

	// Display the Error in Red
	std::cerr << "\033[38;2;255;8;8m";
	// If the image cannot be loaded `loadFromFile` prints an error, I just 'Paint It Red'
	if (!image.loadFromFile(image_path))
	{
		// Reset the Color
		std::cerr << "\033[0m";
		return 3;
	}
	// Reset the Color
	std::cerr << "\033[0m";
	const sf::Vector2u image_size = image.getSize();

	// ASCII Character list of 67 characters ranging from 0 Brightness to 255
	const char symbols[] = " .`^\",:;Il!i~+_-?][}{1)(|/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

	// If Image's Aspect Ratio is greater than 1 work with Width, else work with height;
	float image_aspect = (float)image_size.x / (float)image_size.y;
	// Height is generally 51 but a character █ has double the height than width so it's 102, (term_height*2)
	unsigned short offset_width = image_size.x / term_width--;
	unsigned short offset_height = image_size.y / term_height--;
	if (image_aspect >= 1.f)
	{
		for (uint64_t rows = 0U; rows < image_size.y - offset_height - 1ULL; rows += offset_height + 1ULL)
		{
			if (rows > image_size.y)
				rows = image_size.y - 1;
			for (uint64_t cols = 0U; cols < image_size.x - offset_width - 1ULL; cols += offset_width * 2ULL)
			{
				if (cols > image_size.x)
					cols = image_size.x - 1;
				std::cout << symbols[map((int)getAverageBright(image, cols + rows * image_size.x, offset_width, offset_height), 0, 255, 0, 66)];
			}
			std::cout << std::endl;
		}
	}
	else
	{
		for (uint64_t rows = 0U; rows < image_size.y; rows += offset_height)
		{
			if (rows > image_size.y)
				rows = image_size.y - 1;
			for (uint64_t cols = 0U; cols < image_size.x; cols += offset_width * 2ULL)
			{
				if (cols > image_size.x)
					cols = image_size.x - 1;
				std::cout << symbols[map((int)getAverageBright(image, cols + rows * image_size.x, offset_width, offset_height), 0, 255, 0, 66)];
			}
			std::cout << std::endl;
		}
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

uint8_t getBright(const sf::Color &col)
{
	return uint8_t(0.2126f * col.r + 0.7152f * col.g + 0.0722f * col.b);
}

uint8_t getBright(const uint8_t r, const uint8_t g, const uint8_t b)
{
	return uint8_t(0.2126f * r + 0.7152f * g + 0.0722f * b);
}

uint8_t getAverageBright(const sf::Image &image, unsigned long start_index, const unsigned short &off_x, const unsigned short &off_y)
{
	uint64_t sum = 0ULL;
	for (unsigned int y = (start_index / image.getSize().y); y < (start_index / image.getSize().y) + off_y; ++y)
		for (unsigned int x = (start_index % image.getSize().x); x < (start_index % image.getSize().x) + off_x; ++x)
			sum += getBright(image.getPixel(x, y));

	return uint8_t(sum / (off_x * off_y));
}
