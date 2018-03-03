#ifndef INCLUDE_AGENT_MANAGERS_HPP
#define INCLUDE_AGENT_MANAGERS_HPP

#include "allocore/io/al_App.hpp"
#include "meshes.hpp"
#include "agents.hpp"

struct Capitalist_Entity{
    vector<Capitalist> cs;
    float initial_num;
    Capitalist_Entity(){
        initial_num = 10;
        cs.resize(initial_num);
    }
    Capitalist operator[] (const int index) const{
        return cs[index];
    }
    void run(vector<MetroBuilding>& mbs){
        for (int i = cs.size() - 1; i >= 0; i --){
            Capitalist& c = cs[i];
            c.run(mbs);
        }
    }
    void draw(Graphics& g){
        for (int i = cs.size() - 1; i >= 0; i --){
            Capitalist& c = cs[i];
            c.draw(g);
        }
    }
};

struct Worker_Union{

};

struct Miner_Group{
    vector<Miner> ms;
    float initial_num;

    //visualize relations
    vector<Line> lines;
    bool drawingLinks;

    Miner_Group(){
        initial_num = 30;
        ms.resize(initial_num);
        lines.resize(ms.size());
        drawingLinks = true;

    }
    Miner operator[] (const int index) const{
        return ms[index];
    }
    void run(vector<Natural_Resource_Point>& nrps, vector<Miner>& others){
        for (int i = ms.size() - 1; i >=0; i --){
            Miner& m = ms[i];
            m.run(nrps, others);
        }
        //drawing links
        if (drawingLinks){
            for (int i = ms.size() - 1; i >= 0; i--){
                if (ms[i].resourcePointFound){
                    lines[i].vertices()[0] = ms[i].pose.pos();
                    lines[i].vertices()[1] = nrps[ms[i].id_ClosestNRP].resources[ms[i].id_ClosestResource].position;
                } else {
                    lines[i].vertices()[0] = Vec3f(0,0,0);
                    lines[i].vertices()[1] = Vec3f(0,0,0);
                    // lines[i].vertices()[0] = ms[i].pose.pos();
                    // lines[i].vertices()[1] = nrps[ms[i].id_ClosestNRP].position;
                }
            }
        } else {
            for (int i = ms.size() - 1; i >= 0; i--){
                lines[i].vertices()[0] = Vec3f(0,0,0);
                lines[i].vertices()[1] = Vec3f(0,0,0);
            }
        }
    }
    void bear(){
        //push miner
        //push line
    }
    void die(){
        //delete miner
        //delete line
    }
    void draw(Graphics& g){
        for (int i = ms.size() - 1; i >=0; i --){
            Miner& m = ms[i];
            m.draw(g);
            g.draw(lines[i]);
        }
    }
};

#endif