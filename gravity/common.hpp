#ifndef __COMMON__
#define __COMMON__

#include "allocore/io/al_App.hpp"
using namespace al;

// Common definition of application state
//
struct State {
  Vec3f p_pos[9999];
  Color p_colors[9999];
  //std::vector<Color> particle_colors;
  int particle_number = 0;
  double scaleFactor;
  double sphereRadius = 6;
  double sp_force_value;
  double average_velocity_mag;

};

#endif