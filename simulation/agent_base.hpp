#ifndef INCLUDE_AGENT_BASE_HPP
#define INCLUDE_AGENT_BASE_HPP

#include "allocore/io/al_App.hpp"

struct Agent{
    Vec3f position, velocity, acceleration;
    Pose pose;
    Color c;
    Quatd q;
    Mesh body;
};


#endif