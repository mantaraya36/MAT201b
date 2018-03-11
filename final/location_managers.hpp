#ifndef INCLUDE_LOCATION_MANAGERS_HPP
#define INCLUDE_LOCATION_MANAGERS_HPP

#include "allocore/io/al_App.hpp"
#include "locations.hpp"
#include "meshes.hpp"
#include "status_manager.hpp"
#include "agents.hpp"

struct Miner;

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
    float resourceUnitPrice;
    Factories(){
        drawingLinks = true;
        // initial_num = 30;
        // fs.resize(initial_num);
    }
    void generate(Capitalist_Entity& cs){
        for(int i = cs.cs.size() - 1; i >= 0; i --){
            Factory f;
            Line l;
            f.factoryID = i;
            fs.push_back(f);
            lines.push_back(l);
        }
    }
    void drawLinks(Capitalist_Entity& cs){
        if (drawingLinks){
            for (int i = cs.cs.size() - 1; i >= 0; i --){
                lines[i].vertices()[0] = cs[i].pose.pos();
                lines[i].vertices()[1] = fs[i].position;
            }
        } else{
            for (int i = cs.cs.size() - 1; i >= 0; i --){
                lines[i].vertices()[0] = Vec3f(0,0,0);
                lines[i].vertices()[1] = Vec3f(0,0,0);
            }
        }
    }
    void getResource(Capitalist_Entity& cs){
        for (int i = cs.cs.size() - 1; i >= 0; i --){
            if (cs.cs[i].resourceClock == cs.cs[i].TimeToDistribute - 1){
                fs[i].materialStocks += cs.cs[i].resourceHoldings;
                fs[i].capitalReserve += cs.cs[i].workersPayCheck;
            }
        }
    }
    void getLaborPrice(MarketManager& market){
        for (int i = fs.size() - 1; i >= 0; i --){
            fs[i].resourceUnitPrice = market.resourceUnitPrice;
            fs[i].laborUnitPrice = market.laborUnitPrice; 
            fs[i].MinerCapitalistRatio = market.MinerCapitalistRatio;
        }

    }
    void checkWorkerNum(vector<Worker>& workers){
        for (int i = fs.size() - 1; i >= 0; i --){
            fs[i].workersWorkingNum = 0;
        }
        for (int i = workers.size() - 1; i >= 0; i--){
            if (workers[i].distToClosestFactory <= workers[i].workingDistance){
                fs[workers[i].id_ClosestFactory].workersWorkingNum += 1;
            }
        }
    }
    void payWorkers(MarketManager& market){
        for (int i = fs.size() - 1; i >= 0; i--){
            fs[i].individualSalary = market.laborUnitPrice / 60;
            fs[i].capitalReserve -= fs[i].workersWorkingNum * market.laborUnitPrice / 360;
        }
    }

    void bankrupt(){
        //capitalist won't die, instead the factory dies
    }

    void run(Capitalist_Entity& cs){
        getResource(cs);
        for (int i = fs.size() - 1; i >= 0; i --){
            Factory& f = fs[i];
                //rather than destroy the object, stop animating
                //fs.erase(fs.begin() + i);
            f.run();
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

struct NaturalResourcePointsCollection {
    vector<Natural_Resource_Point> nrps;
    int initial_num;
    NaturalResourcePointsCollection(){
        initial_num = 25;
        nrps.resize(initial_num);
    }

    void checkMinerPick(vector<Miner>& miners){
        for (int k = nrps.size() - 1; k >= 0; k --){
            Natural_Resource_Point& nrp = nrps[k];
            for (int i = 0; i < nrp.resources.size(); i ++){
                nrp.resources[i].beingPicked = false;
                
            }
        }
        for (int j = 0; j < miners.size(); j ++){
                if (miners[j].distToClosestResource < miners[j].pickingRange && miners[j].resourcePointFound == true){
                    nrps[miners[j].id_ClosestNRP].resources[miners[j].id_ClosestResource].beingPicked = true;
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