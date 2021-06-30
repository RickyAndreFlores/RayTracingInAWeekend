#include <iostream>
#include "vec3.h"
#include "ray.h"


void write_color(std::ostream& out, color pixel_color) 
{
	// Write the translated [0,255] value of each color component.
	out << static_cast<int>(255.999 * pixel_color.x()) << ' '
		<< static_cast<int>(255.999 * pixel_color.y()) << ' '
		<< static_cast<int>(255.999 * pixel_color.z()) << '\n'; 
}
double hit_sphere(const point3& center, double radius, const ray& r) {
	// solve the equation (P(t) - center)(P(t) - center) = r2 using quadratic equation, the number of roots signify number of intersections on sphere
	// The equation of a circle around origin, is P.dot(P) = r^2, where P is a vec3 on the surface of that sphere 
	// equation of a circle around a center is (P-C).dot(P-C) = r^2
	// set P, the vec3 point on the sphere equal to the ray equation that returns a vec3 point: P(t) = origin + t *dir
	// convert it to a form friendly for quadratic equation, and solve for t ; if square root part > 0, 2 hits through sphere (front and back), < 0 no hits, == 0 1 hit (tangent to surface)

	vec3 oc = r.origin() - center;
	double a = r.direction().length_squared();
	double half_b = dot(oc, r.direction());
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;

	if (discriminant < 0) {
		return -1.0;
	}
	else {
		return (-half_b - sqrt(discriminant)) / a;
	}
}

color ray_color(const ray& r) 
{
	double t = hit_sphere(point3(0, 0, -1), 0.5, r);
	// if hit
	if (t > 0.0) {

		vec3 center_to_hit = r.at(t) - vec3(0, 0, -1);
		vec3 normal_unit = unit_vector(center_to_hit);
		// convert from [-1,1] to [0,1]
		return 0.5 * color(normal_unit.x() + 1, normal_unit.y() + 1, normal_unit.z() + 1);
	}
	//else 
	vec3 unit_direction = unit_vector(r.direction());
	double height_percent = 0.5 * (unit_direction.y() + 1.0); // convert to [0,1] range from [-1,1]
	return (1.0 - height_percent) * color(1.0, 1.0, 1.0) + height_percent * color(0.5, 0.7, 1.0); // linearly interporlate based off height
}


int main()
{




	const double aspect_ratio = 16.0 / 9.0; //  == width / height 
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio); // == width * height / width 

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
			double percent_width  = double(col) / (image_width - 1);
			double percent_height = double(row) / (image_height - 1);

			vec3 pixel_position = lower_left_corner + percent_width * horizontal + percent_height * vertical;
			vec3 direction = pixel_position - origin;

			ray r(origin, direction);
			color pixel_color = ray_color(r);

			write_color(std::cout, pixel_color);
		}
	}
	std::cerr << "\nDone.\n";

}