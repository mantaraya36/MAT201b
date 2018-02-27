#ifndef INCLUDE_LOCATION_MANAGERS_HPP
#define INCLUDE_LOCATION_MANAGERS_HPP

#include "allocore/io/al_App.hpp"
#include "locations.hpp"
#include "meshes.hpp"
#include "status_manager.hpp"

struct Metropolis{    
    vector<MetroBuilding> mbs;
    int initial_num;
    Metropolis(){
        initial_num = 25;
        mbs.resize(initial_num);
    }

    MetroBuilding operator [](const int index) const {
        return mbs[index];
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
    vector<Line> lines;
    int initial_num;
    bool drawingLinks;
    Factories(){
        drawingLinks = true;
        // initial_num = 30;
        // fs.resize(initial_num);
    }
    void generate(Capitalist_Entity& cs){
        for(int i = cs.cs.size() - 1; i >= 0; i --){
            Factory f;
            Line l;
            fs.push_back(f);
            lines.push_back(l);

        }
    }
    void drawLinks(Capitalist_Entity& cs){
        if (drawingLinks){
            for(int i = cs.cs.size() - 1; i >= 0; i --){
                lines[i].vertices()[0] = cs[i].pose.pos();
                lines[i].vertices()[1] = fs[i].position;
            }
        } else{
            for(int i = cs.cs.size() - 1; i >= 0; i --){
                lines[i].vertices()[0] = Vec3f(0,0,0);
                lines[i].vertices()[1] = Vec3f(0,0,0);
            }
        }
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
            g.draw(lines[i]);
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

    void PickEventHandler(EventManager& eventManager){
        for (int i = nrps.size() - 1; i >=0; i--){
            for (int j = nrps[i].resources.size() - 1; j >=0; j--){
                    nrps[i].resources[j].beingPicked = eventManager.nrps[i].resource_picked[j];           
            }
        }
    }
    
    void run(){
        for (int i = nrps.size() - 1; i >= 0; i --){
            Natural_Resource_Point& nrp = nrps[i];
            nrp.respawn_resource();
            nrp.update_resource();
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