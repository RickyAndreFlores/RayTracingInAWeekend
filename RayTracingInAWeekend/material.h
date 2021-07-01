#pragma once
#include "rtweekend.h"

#include "hittable.h"

class material {
public:
    // function to describe behavior of light after hitting object, should it bounce , if so how so
    virtual bool scatter( const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};


class lambertian : public material {
public:
    lambertian(const color& a) : albedo(a) {}

    virtual bool scatter( const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        // get new diffuse bounce direction 
        // by choosing a unit vector we choose a point on the surface of the unit sphere, therefroe makine the probabiltiy of ray sacttering to thetha more like true lambertian 
        // if we used a random vector within the unit sphere like before there are many more chances for a ray to be in the unit sphere (the area is larger than surface) making the probablity much higher to be close to normal
        // by using a random unit vector + normal we choose point on surface, and therefer increas probability of it going away from normal, leading to more indirect light bounces to camera making diffuse materails (whose only source of light is that) lighter
        vec3 scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction, avoids error where diffuse bounce collapsing back to the point (zero direction) 
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        // new direction is a ray that starts from the hit point and going in (normal direction +  random unit vector within a unit sphere)
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;

        return true;
    }

public:
    color albedo;
};


class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override 
    {
        // get reflected ray direction
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        reflected = reflected + fuzz * random_in_unit_sphere();
        // refected rat starts at hitpoint
        scattered = ray(rec.p, reflected);
        // set albedo
        attenuation = albedo;
        // return true (does scatter, only if scattered direction is in same hempisphere as normal (pointing outwards)
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz;

};


class dielectric : public material {
public:
    // dielectric means light both refracts through the material and reflects in some way

    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter( const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override 
    {

        attenuation = color(1.0, 1.0, 1.0);
        // refraction ratio depends on whether it is entering material or exiting material, essentialy goes back to normal angle after leaving
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        // unit vector or ray in direction
        vec3 unit_direction = unit_vector(r_in.direction());
        // refract 
        vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);

        // make ray from hit point to refracted (goes through material at an new angle
        scattered = ray(rec.p, refracted);
        return true;
    }

public:
    double ir; // Index of Refraction
};
