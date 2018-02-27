#ifndef INCLUDE_STATUS_MANAGER_HPP
#define INCLUDE_STATUS_MANAGER_HPP

#include "allocore/io/al_App.hpp"
#include "agents.hpp"
#include "locations.hpp"

using namespace al;
using namespace std;


struct NRPAlias{
    vector<bool> resource_picked;
    NRPAlias(){
        resource_picked.resize(100);
        
        for (bool b : resource_picked){
            b = false;
        }
    }
};

struct EventManager{
    vector<NRPAlias> nrps;
    vector<bool> nrps_approached;

    EventManager(){
        nrps.resize(100);
        nrps_approached.resize(100);
    }

    void updateCollectingStatus(vector<Miner>& ms){
        for(int i = nrps.size() - 1; i >=0; i--){
            for (int j = nrps[i].resource_picked.size() - 1; j >=0; j--){
                nrps[i].resource_picked[j] = false;
            }
        }

        for (int i = ms.size() - 1; i >=0; i--){
            if (ms[i].resourcePointFound){
                //nrps_approached[ms[i].id_ClosestNRP] = true;
                //nrps_resources_collecting[ms[i].id_ClosestResource] = true;
                nrps[ms[i].id_ClosestNRP].resource_picked[ms[i].id_ClosestResource] = true;
            }
        }

    }
    
};


#endif