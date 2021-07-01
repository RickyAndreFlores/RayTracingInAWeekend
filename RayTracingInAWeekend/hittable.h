#pragma once

#include "rtweekend.h"

class material; 

struct hit_record {
    point3 p;      
    vec3 normal; // normal from hit point
    shared_ptr<material> mat_ptr;
    double t; // ray = P(t) = origin + t * dir

    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        // The dot product of two vector is essentially the length of the projetion scaled by the other vector
        // so mathematically it comes out that if it is negative, the two vectors are of pointing to different sides (one vector scaled it negatively) 
        front_face = dot(r.direction(), outward_normal) < 0;
        // change direction of normal based on if we hit the inside or outside of the sphere
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};