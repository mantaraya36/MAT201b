#ifndef INCLUDE_AGENTS_HPP
#define INCLUDE_AGENTS_HPP

#include "allocore/io/al_App.hpp"
#include "allocore/math/al_Quat.hpp"
#include "allocore/spatial/al_Pose.hpp"
#include "helper.hpp"
#include "agent_base.hpp"
#include "locations.hpp"

using namespace al;
using namespace std;

struct Resource;
struct Factory;
struct Natural_Resource_Point;
struct MetroBuilding;

struct Worker : Agent {
    
    Worker(){

    }

    void seekFactory(const vector<Factory>& f){

    }


};

struct Capitalist : Agent {

    Capitalist(){

    }
};

#include "locations.hpp"

#endif