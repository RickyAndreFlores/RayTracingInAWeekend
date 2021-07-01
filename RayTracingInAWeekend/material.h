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
    metal(const color& a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override 
    {
        // get reflected ray direction
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        // refected rat starts at hitpoint
        scattered = ray(rec.p, reflected);
        // set albedo
        attenuation = albedo;
        // return true (does scatter, only if scattered direction is in same hempisphere as normal (pointing outwards)
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
};