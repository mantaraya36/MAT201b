#ifndef INCLUDE_LOCATION_MANAGERS_HPP
#define INCLUDE_LOCATION_MANAGERS_HPP

#include "allocore/io/al_App.hpp"
#include "locations.hpp"

struct Metropolis{
    vector<MetroBuilding> mbs;
    int initial_num;
    Metropolis(){
        initial_num = 25;
        mbs.resize(initial_num);
    }
    void run(){
        for (MetroBuilding& mb : mbs){
            mb.floating();
        }
    }
    void draw(Graphics& g){
        for (int i = mbs.size() - 1; i >= 0; i --){
            MetroBuilding& mb = mbs[i];
            mb.draw(g);
        }
    }

};

struct Factories {
    vector<Factory> fs;
    int initial_num;
    Factories(){
        initial_num = 30;
        fs.resize(initial_num);
    }

    void run(){
        for (int i = fs.size() - 1; i >= 0; i --){
            Factory& f = fs[i];
            //bankrupt check before update
            if (f.operating()){
                //rather than destroy the object, stop animating
                //fs.erase(fs.begin() + i);
                f.produce();
                f.animate();
            }
        }
    }

    void draw(Graphics& g){
        for (int i = fs.size() - 1; i >= 0; i --){
            Factory& f = fs[i];
            f.draw(g);
        }
    }
};

struct NRPs {
    vector<Natural_Resource_Point> nrps;
    int initial_num;
    NRPs(){
        initial_num = 25;
        nrps.resize(initial_num);
    }
    
    void run(){
        for (int i = nrps.size() - 1; i >= 0; i --){
            Natural_Resource_Point& nrp = nrps[i];
            nrp.respawn_resource();
            nrp.update_resource();
            //drain check
            if (nrp.drained()){
                nrps.erase(nrps.begin() + i);
            }
        }
    }
    void draw(Graphics& g){
        for (int i = nrps.size() - 1; i >= 0; i --){
            Natural_Resource_Point& nrp = nrps[i];
            nrp.draw(g);
        }
    }
};



#endif