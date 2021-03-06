#pragma once

#include <cmath>
#include <iostream>


class vec3 {
public:

    double e[3];

    vec3() : e{ 0,0,0 } {}
    vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(const double t) {
        return *this *= 1 / t;
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }


    inline static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    inline static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        // this is useful in avoiding float point error leading to a near zero value when it should just be zero
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }
};

// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color



// vec3 Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
        + u.e[1] * v.e[1]
        + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}



vec3 random_in_unit_sphere() {
    // brute force find a random vector within a unit circle, i.e length < 1 
    // this is just a tutorial, don't judge me
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}


vec3 random_unit_vector()
{
    // indirectly a brute force way to get a random unit vector
    return unit_vector(random_in_unit_sphere());

}

vec3 reflect(const vec3& incoming_ray_dir, const vec3& normal) 
{
    // due to the geometry of a virtual reflection, we can do some math to conclude 
    // that reflected_ray_dir = incoming_ray_dir + 2b ; where ||b|| =  incoming_ray_dir.dot(normal)
    // b = a vector pointng in the direction of the normal (a unit vector), with length ||b||, so to get that we do ||b|| * normal
    // since incoming and normal are pointing to oppsite hemispheres, the dot will be negative, so make it positive by multipling by -1
    return incoming_ray_dir + 2 * (dot(incoming_ray_dir, normal) * -1) * normal;
}



vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {


    // using snells law normal * sin(theta) = normal_prime * sin(theta_prime)
    // etai_over_etat = n/n' = normal / normal_prime
    // using some maths and snells equation we can come to these equations for the vectors that make up the refracted ray vector

    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;

    // vector addition of the perendical and parrallel componets (the x and y component vectors) result in final refracted ray vector 
    return r_out_perp + r_out_parallel;
}


vec3 random_in_unit_disk() {
    while (true) {
        // get a slice in 2D space
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}