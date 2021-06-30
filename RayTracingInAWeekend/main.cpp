

#include "rtweekend.h"


#include "color.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>

color ray_color(const ray& r, const hittable& world) {
	hit_record rec;

	if (world.hit(r, 0, infinity, rec)) {
		return 0.5 * (rec.normal + color(1, 1, 1)); // convert to [0,1] range from [-1,1]
	}

	//else  color background / miss
	vec3 unit_direction = unit_vector(r.direction());
	double height_percent = 0.5 * (unit_direction.y() + 1.0); // convert to [0,1] range from [-1,1]
	return (1.0 - height_percent) * color(1.0, 1.0, 1.0) + height_percent * color(0.5, 0.7, 1.0); // linearly interporlate based off height
}


int main()
{
	//Image
	const double aspect_ratio = 16.0 / 9.0; //  == width / height 
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio); // == width * height / width 

	// World
	hittable_list world;
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100)); // bigger sphere just touching

	// Camera
	double viewport_height = 2.0;
	double viewport_width = aspect_ratio * viewport_height; // == width / height  * height in ndc
	double focal_length = 1.0; // distance from projection plane and projection point
		
	// get normalized device coordinates
	vec3 origin		= point3(0, 0, 0);
	vec3 horizontal = vec3(viewport_width, 0, 0); // length not starting point
	vec3 vertical	= vec3(0, viewport_height, 0);

	// subtract by half width and half vertical to get to lower left hand side, then subtract focal length to go to z where the projection plane is (-z is forward)
	vec3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);


	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int row = image_height - 1; row >= 0; --row)
	{
		std::cerr << "\rScanlines remaining: " << row << ' ' << std::flush;

		for (int col = 0; col < image_width; ++col)
		{
			double percent_width  = double(col) / (image_width - 1.0);
			double percent_height = double(row) / (image_height - 1.0);

			vec3 pixel_position = lower_left_corner + percent_width * horizontal + percent_height * vertical;
			vec3 direction = pixel_position - origin;

			ray r(origin, direction);
			color pixel_color = ray_color(r, world);

			write_color(std::cout, pixel_color);
		}
	}
	std::cerr << "\nDone.\n";

}