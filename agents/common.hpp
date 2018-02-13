#ifndef __COMMON__
#define __COMMON__

#include "allocore/io/al_App.hpp"
using namespace al;

// Common definition of application state
//
struct State {
  Vec3f boid_pos[9999];
  Quatf boid_quat[9999];
  Color boid_color[9999];
  Vec3f t_pos[9999];
  Color t_color[9999];
  int target_number = 0;
  int boid_number = 0;
  double scaleFactor;
  double coneRadius = 3;
  double coneHeight = coneRadius * 3;
  double sphereRadius = 3;
  double average_velocity;
  double solitude_level;

};

#endif