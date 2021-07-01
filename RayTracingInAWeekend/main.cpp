

#include "rtweekend.h"


#include "color.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>

#include "camera.h"


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
	const int samples_per_pixel = 100;

	// World
	hittable_list world;
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100)); // bigger sphere just touching

	camera cam;

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	for (int row = image_height - 1; row >= 0; --row)
	{
		std::cerr << "\rScanlines remaining: " << row << ' ' << std::flush;
		for (int col = 0; col < image_width; ++col)
		{
			color pixel_color(0, 0, 0); // set inital color to zero
			for (int s = 0; s < samples_per_pixel; ++s) 
			{
				// get coordinates in pixel space, for current_pixel + some random vector whose components are [0,1] 
				double pixel_u = (col + random_double()) / (image_width - 1);
				double pixel_v = (row + random_double()) / (image_height - 1);
				// create a ray from camera origin, pointing to that pixel
				ray r = cam.get_ray(pixel_u, pixel_v);
				pixel_color += ray_color(r, world); // for each sample add color
			}
			// bring color total back to normal by weighting it 1 / samples_per_pixel  
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}
	std::cerr << "\nDone.\n";

}