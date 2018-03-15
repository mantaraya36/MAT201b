#ifndef __COMMON__
#define __COMMON__

#include "allocore/io/al_App.hpp"

using namespace al;

struct State{
    //general stats
    float numMiners;
    float numCapitalists;
    float numWorkers;
    float numResourcePoints;

    //miner
    Pose miner_pose[100];
    float miner_scale[100];
    float miner_poetryHoldings[100];
    bool miner_bankrupted[100];
    bool miner_fullpack[100];

    //worker
    Pose worker_pose[75];
    float worker_scale[75];
    float worker_poetryHoldings[75];
    bool worker_bankrupted[75];

    //capitalist
    Pose capitalist_pose[15];
    float capitalist_scale[15];
    float capitalist_poetryHoldigs[15];
    bool capitalist_bankrupted[15];

    //factory
    Vec3f factory_pos[15];
    float factory_rotation_angle[15];
    float factory_size[15];
    Color factory_color[15];

    //resources
    Vec3f resource_point_pos[25]; 
    Vec3f resource_pos[25 * 7]; // the positions of resource are already global
    bool resource_visible[25 * 7];

    //nav
    // Vec3f nav_pos;
    // Quatf nav_quat;
    Pose nav_pose;

    //buildings
    Vec3f building_pos[15];
    float building_size[15];

};

#endif