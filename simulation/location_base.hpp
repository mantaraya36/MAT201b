#ifndef INCLUDE_LOCATION_BASE_HPP
#define INCLUDE_LOCATION_BASE_HPP

#include "allocore/io/al_App.hpp"

struct Location{
    Vec3f position;
    float scaleFactor;
    Color c;
    Mesh mesh;
};

#endif