

#include "rtweekend.h"


#include "color.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>

#include "camera.h"


color ray_color(const ray& r, const hittable& world, int depth) {
	hit_record rec;
	
	// if we reach max number of bounces, no more color is gathered
	if (depth <= 0)
	{
		return color(0, 0, 0);
	}


	// avoid floating point error by making min = 0 + e ; makes reflected rays not hit the same object when bouncing
	if (world.hit(r, 0.001, infinity, rec)) {
		// instead of just returning the normal converted to a color, get new diffuse bounce direction and recursively bounce
		// new direction is a ray that starts from the hit point and going in (normal direction +  random unit vector within a unit sphere)
		// by choosing a unit vector we choose a point on the surface of the unit sphere, therefroe makine the probabiltiy of ray sacttering to thetha more like true lambertian 
		// if we used a random vector within the unit sphere like before there are many more chances for a ray to be in the unit sphere (the area is larger than surface) making the probablity much higher to be close to normal
		// by using a random unit vector + normal we choose point on surface, and therefer increas probability of it going away from normal, leading to more indirect light bounces to camera making diffuse materails (whose only source of light is that) lighter
		point3 target = rec.p + rec.normal + random_unit_vector();

		return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
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
				pixel_color += ray_color(r, world, max_depth); // for each sample add color
			}
			// bring color total back to normal by weighting it 1 / samples_per_pixel  
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}
	std::cerr << "\nDone.\n";

}