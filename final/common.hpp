#ifndef __COMMON__
#define __COMMON__

#include "allocore/io/al_App.hpp"

struct State{
    //miner
    Pose miner_pose[120];
    float miner_poetryHoldings[120];
    bool miner_bankrupted[120];
    bool miner_fullpack[120];

    //worker
    Pose worker_pose[120];
    float worker_poetryHoldings[90];
    bool worker_bankrupted[120];

    //capitalist
    Pose capitalist_pose[120];
    float capitalist_size[15];
    float capitalist_poetryHoldigs[15];
    bool capitalist_bankrupted[15];

    //factory
    Vec3f factory_pos[15];
    float factory_rotation_angle[15];
    float factory_size[15];
    Color factory_color[15];

    //resources
    Vec3f resource_point_pos[25]; 
    Vec3f resource_pos[25 * 8]; // the positions of resource are already global
    bool resource_visible[25 * 8];

    //nav
    // Vec3f nav_pos;
    // Quatf nav_quat;
    Pose nav_pose;

    //buildings
    Vec3f building_pos[15];
    float building_size[15];

};

#endif