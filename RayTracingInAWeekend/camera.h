#pragma once

#include "rtweekend.h"

class camera {
public:
    camera(point3 lookfrom,
            point3 lookat,
            vec3   vup, 
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio, 
            double aperture,
            double focus_dist
          )
    {
        // change field of view
        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2); // height from origin plane (x,y, 0), to top of viewport. 
        double viewport_height = 2.0 * h; // Since viewport center is pointing towards origin (half way in negative), * 2
        double viewport_width = aspect_ratio * viewport_height; // == width / height  * height in ndc

        w = unit_vector(lookfrom - lookat); // vector from project plane to camera point . ||w|| == focal length
        u = unit_vector(cross(vup, w)); // get unit vector orthgonal to w and vup == vector in local horizontal directional ; v up can be any angle to w and it still works, vup == global up vector
        v = cross(w, u); // now that w and u are perpendicular, getting the cross again gives the local vertical unit direction vector; leading to having all 3 local axises
        
        // these are all vectors 
        origin = lookfrom;

        horizontal = viewport_width * u; // vector pointing in local horziontal direction of length viewport_width
        horizontal = focus_dist * horizontal; // if focus distance is twice as far, then because of projection, the projection plane is twice as large
        vertical = viewport_height * v; // vector pointing in local vertical direction of length viewport_height
        vertical = focus_dist * vertical; // vertical scales with focus distance (the closer focus dist is the smaller the projection plane 
          // subtract by half width and half vertical to get to lower left hand side, then subtract by w (which is length == focal length and in direction to plane) to go to z where the projection plane is (-z is forward)
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
    }

    ray get_ray(double s, double t) const {

        // get random point in unit disk, on plane with axises (u,v)
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();

        // make ray originate from that offset on camera plane; this is a point in the'lens' 
        return ray( origin + offset,
                    // get ray pointing towards pixels s,t starting at origin
                    lower_left_corner + s * horizontal + t * vertical - origin - offset
        );


        return ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    vec3 w;
    vec3 u;
    vec3 v;
    double lens_radius;
};
