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
        // when the ray is in the material with the higher refractive index, there is no real solution to Snell’s law
        attenuation = color(1.0, 1.0, 1.0);
        // refraction ratio depends on whether it is entering material or exiting material, essentialy goes back to normal angle after 
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        // unit vector or ray in direction
        vec3 unit_direction = unit_vector(r_in.direction());

        // using snells law we can find when the equation has no solution:  sin(theta) = refration_ratio * sin'(theta') when right hand side > 1 (max of sin is 1) 
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0); // calculcate sign by using cosign ans dot product properties
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;

        vec3 direction;
        
        // if cant reflect due to snells law, or schlicks approximation of reflectance
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        // make ray from hit point to direction of either reflected or refracted ray
        scattered = ray(rec.p, direction);
        return true;
    }

public:
    double ir; // Index of Refraction


private:
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};
