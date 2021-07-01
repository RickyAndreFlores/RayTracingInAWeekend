#pragma once

#include "rtweekend.h"

class camera {
public:
    camera(point3 lookfrom,
            point3 lookat,
            vec3   vup, 
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio) 
    {
        // change field of view
        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2); // height from origin plane (x,y, 0), to top of viewport. 
        double viewport_height = 2.0 * h; // Since viewport center is pointing towards origin (half way in negative), * 2
        double viewport_width = aspect_ratio * viewport_height; // == width / height  * height in ndc


        vec3 w = unit_vector(lookfrom - lookat); // vector from project plane to camera point . ||w|| == focal length
        vec3 u = unit_vector(cross(vup, w)); // get unit vector orthgonal to w and vup == vector in local horizontal directional ; v up can be any angle to w and it still works, vup == global up vector
        vec3 v = cross(w, u); // now that w and u are perpendicular, getting the cross again gives the local vertical unit direction vector; leading to having all 3 local axises

        origin = lookfrom;
        horizontal = viewport_width * u; // vector pointing in local horziontal direction of length viewport_width
        vertical = viewport_height * v; // vector pointing in local vertical direction of length viewport_height
          // subtract by half width and half vertical to get to lower left hand side, then subtract by w (which is length == focal length and in direction to plane) to go to z where the projection plane is (-z is forward)
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;


    }

    ray get_ray(double s, double t) const {
        // get ray pointing towards pixels s,t starting at origin
        return ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};
