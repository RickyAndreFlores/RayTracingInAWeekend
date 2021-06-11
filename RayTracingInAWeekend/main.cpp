#include <iostream>


int main()
{

	const int image_width = 256;
	const int image_height = 256;


	for (int row = image_height - 1; row >= 0; --row)
	{
		for (int col = 0; col < image_width; --col)
		{

			double percent_width = col / (double)(image_width - 1);
			double percent_height = row / (double)(image_height - 1);
			double b_percent = 0.25;

			int r = static_cast<int>(255.999 * percent_width);
			int g = static_cast<int>(255.999 * percent_height);
			int g = static_cast<int>(255.999 * b_percent);

			std::cout << r << ' ' << g << ' ' << b << '\n';

		}

	}
}