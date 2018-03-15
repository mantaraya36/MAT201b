#ifndef __COMMON__
#define __COMMON__

#include "allocore/io/al_App.hpp"

using namespace al;

struct State{
    //general stats
    float numMiners = 100;
    float numCapitalists = 15;
    float numWorkers = 75;
    float numResources = 25 * 7;

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
    Quatf factory_facing_center[15];
    float factory_size[15];
    Color factory_color[15];

    //resources
    Vec3f resource_point_pos[25]; 
    Vec3f resource_pos[25 * 7]; // the positions of resource are already global
    float resource_angleA[25 * 7];
    float resource_angleB[25 * 7];
    float resource_scale[25 * 7];
    bool resource_picked[25 * 7];

    //lines
    Vec3f worker_lines_posA[75];
    Vec3f worker_lines_posB[75];
    Vec3f capitalist_lines_posA[15];
    Vec3f capitalist_lines_posB[15];

    //nav
    // Vec3f nav_pos;
    // Quatf nav_quat;
    Pose nav_pose;

    //buildings
    float metro_rotate_angle;
    Vec3f building_pos[15];
    float building_size[15];

};

#endif