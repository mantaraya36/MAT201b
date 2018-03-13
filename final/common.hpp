#ifndef __COMMON__
#define __COMMON__

#include "allocore/io/al_App.hpp"

struct State{
    //miner
    Vec3f miner_pos[120];
    Quatf miner_quat[120];
    float miner_poetryHoldings[120];
    bool miner_bankrupted[120];

    //worker
    Vec3f worker_pos[90];
    Quatf worker_quat[90];
    float worker_poetryHoldings[90];
    bool worker_bankrupted[120];

    //capitalist
    Vec3f capitalist_pos[15];
    Quatf capitalist_quat[15];
    float capitalist_poetryHoldigs[15];
    bool capitalist_bankrupted[15];

    //factory
    Vec3f factory_pos[15];
    float factory_rotation_angle[15];
    float factory_size[15];
    Color factory_color[15];

    //resources
    Vec3f resource_point_pos[25]; 
    Vec3f resource_pos[25 * 8];

    //buildings
    Vec3f building_pos[15];

}

#endif