

#include "rtweekend.h"


#include "color.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>

#include "camera.h"
#include "material.h"

hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.0, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {

					// diffuse
					auto albedo = color::random() * color::random();

					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}


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
	const double aspect_ratio = 3.0 / 2.0; //  == width / height 
	const int image_width = 300;
	const int image_height = static_cast<int>(image_width / aspect_ratio); // == width * height / width 
	const int samples_per_pixel = 10;
	const int max_depth = 10;

	// World
	hittable_list world = random_scene();

	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	double dist_to_focus = 10.0;
	double aperture = 0.1;

	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);


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
				double pixel_u = (col + random_double()) / (image_width - 1.0);
				double pixel_v = (row + random_double()) / (image_height - 1.0);
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