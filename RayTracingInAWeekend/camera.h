#pragma once

#include "rtweekend.h"

class camera {
public:
    camera() {
        // Camera
        double aspect_ratio = 16.0 / 9.0;
        double viewport_height = 2.0;
        double viewport_width = aspect_ratio * viewport_height; // == width / height  * height in ndc
        double focal_length = 1.0; // distance from projection plane and projection point
        
        origin = point3(0, 0, 0);

        // length in normalized device coordinates
        horizontal = vec3(viewport_width, 0.0, 0.0);// length not starting point
        vertical = vec3(0.0, viewport_height, 0.0);
        // subtract by half width and half vertical to get to lower left hand side, then subtract focal length to go to z where the projection plane is (-z is forward)
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);
    }

    ray get_ray(double u, double v) const {
        // get ray pointing towards pixels u,v starting at origin
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};
