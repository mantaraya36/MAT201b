#ifndef INCLUDE_AGENT_MANAGERS_HPP
#define INCLUDE_AGENT_MANAGERS_HPP

#include "allocore/io/al_App.hpp"
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
    Miner_Group(){
        initial_num = 30;
        ms.resize(initial_num);
    }
    Miner operator[] (const int index) const{
        return ms[index];
    }
    void run(vector<Natural_Resource_Point>& nrps, vector<Miner>& others){
        for (int i = ms.size() - 1; i >=0; i --){
            Miner& m = ms[i];
            m.run(nrps, others);
        }
    }
    void draw(Graphics& g){
        for (int i = ms.size() - 1; i >=0; i --){
            Miner& m = ms[i];
            m.draw(g);
        }
    }
};

#endif