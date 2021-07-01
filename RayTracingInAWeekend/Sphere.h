#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere() {}
    sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m){};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
    point3 center = point3(0,0,-1);
    double radius = 0.0;
    shared_ptr<material> mat_ptr;

};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

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

    // if discriminant is negative there are no solution and no hit
    if (discriminant < 0)
    {
        return false;
    }
    //else 
    double sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range
    double root = (-half_b - sqrtd) / a;
    // if not in acceptable range try next root
    if (root < t_min || t_max < root) 
    {
        root = (-half_b + sqrtd) / a;
        // if not in accepetable range
        if (root < t_min || t_max < root)
            return false;
    }

    // make hit record+
    rec.t = root;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / radius;
    // calculate normal, make unit vector
    vec3 outward_normal = (rec.p - center) / radius;
    // flips it in the correct direction
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;


    return true;
}