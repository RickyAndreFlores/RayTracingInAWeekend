

#include "rtweekend.h"


#include "color.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>

#include "camera.h"
#include "material.h"


color ray_color(const ray& r, const hittable& world, int depth) {
	hit_record rec;
	
	// if we reach max number of bounces, no more color is gathered
	if (depth <= 0)
	{
		return color(0, 0, 0);
	}


	// avoid floating point error by making min = 0 + e ; makes reflected rays not hit the same object when bouncing
	if (world.hit(r, 0.001, infinity, rec)) {
		ray scattered;
		color attenuation;
		// if light bounces back , instead of getting absorbed ( bounces away from normal), get behavior based on material hit and the recursively bounce ray again
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			// instead of just returning the normal converted to a color, get new diffuse bounce direction and recursively bounce
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}

	// since this is diffuse, the only time this actually gets color is if it misses and get color from surounding background (and modulate )
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
	const int max_depth = 50;

	// World
	hittable_list world;


	shared_ptr<lambertian> material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	shared_ptr<lambertian> material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	shared_ptr<metal> material_left = make_shared<metal>(color(0.8, 0.8, 0.8), .3);
	shared_ptr<metal> material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground)); // bigger sphere just touching smaller ones
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));


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
				pixel_color += ray_color(r, world, max_depth); // for each sample add color
			}
			// bring color total back to normal by weighting it 1 / samples_per_pixel  
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}
	std::cerr << "\nDone.\n";

}