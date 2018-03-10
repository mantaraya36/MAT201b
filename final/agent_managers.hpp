#ifndef INCLUDE_AGENT_MANAGERS_HPP
#define INCLUDE_AGENT_MANAGERS_HPP

#include "allocore/io/al_App.hpp"
#include "meshes.hpp"
#include "agents.hpp"

struct Capitalist_Entity{
    vector<Capitalist> cs;
    int initial_num;
    Capitalist_Entity(){
        initial_num = 30;
        cs.resize(initial_num);
    }
    Capitalist operator[] (const int index) const{
        return cs[index];
    }
    void initID(){
        for (int i = cs.size() - 1; i >= 0; i --){
            cs[i].capitalistID = i;
        }
    }
    void getResource(vector<Miner>& miners){
        for (int i = cs.size() - 1; i >= 0; i --){
            for (int j = miners.size() - 1; i >= 0; i --){
                 if (i == miners[j].id_ClosestCapitalist){
                    if (miners[j].tradeTimer == miners[j].unloadTimeCost - 1){
                        cs[i].resourceHoldings += miners[j].resourceHoldings;
                    }
                 }
            }
        }
    }
    void getWorkersPaymentStats(vector<Factory>& fs){
        for (int i = cs.size() - 1; i >= 0; i --){
            cs[i].numWorkers = fs[i].workersWorkingNum;
            cs[i].workersPayCheck = fs[i].laborUnitPrice * cs[i].numWorkers;
        }
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
    vector<Worker> workers;
    int initial_num;

    //visualize relations;
    vector<Line> lines;
    bool drawingLinks;

    Worker_Union(){
        initial_num = 60;
        workers.resize(initial_num);
        lines.resize(workers.size());
        drawingLinks = true;
    }
    Worker operator[] (const int index) const{
        return workers[index];
    }
    void run(vector<Factory>& fs, vector<Worker>& others, vector<Capitalist>& capitalist){
        for (int i = workers.size() - 1; i >= 0; i --){
            Worker& w = workers[i];
            w.run(fs, others, capitalist);
        }
        visualize(fs);
    }
    void visualize(vector<Factory>& fs){
        if (drawingLinks){
            for (int i = workers.size() - 1; i >= 0; i--){
                if (workers[i].FactoryFound){
                    lines[i].vertices()[0] = workers[i].pose.pos();
                    lines[i].vertices()[1] = fs[workers[i].id_ClosestFactory].position;
                } else {
                    lines[i].vertices()[0] = Vec3f(0,0,0);
                    lines[i].vertices()[1] = Vec3f(0,0,0);
                    // lines[i].vertices()[0] = ms[i].pose.pos();
                    // lines[i].vertices()[1] = nrps[ms[i].id_ClosestNRP].position;
                }
            }
        } else {
            for (int i = workers.size() - 1; i >= 0; i--){
                lines[i].vertices()[0] = Vec3f(0,0,0);
                lines[i].vertices()[1] = Vec3f(0,0,0);
            }
        }
    }
    void draw(Graphics& g){
        for (int i = workers.size() - 1; i >=0; i --){
            Worker& w = workers[i];
            w.draw(g);
            g.draw(lines[i]);
        }
    }
};

struct Miner_Group{
    vector<Miner> ms;
    int initial_num;

    //visualize relations
    vector<Line> lines;
    bool drawingLinks;

    Miner_Group(){
        initial_num = 60;
        ms.resize(initial_num);
        lines.resize(ms.size());
        drawingLinks = true;

    }
    Miner operator[] (const int index) const{
        return ms[index];
    }
    void run(vector<Natural_Resource_Point>& nrps, vector<Miner>& others, vector<Capitalist>& capitalists){
        for (int i = ms.size() - 1; i >=0; i --){
            Miner& m = ms[i];
            m.run(nrps, others, capitalists);
        }

        //drawing links
        visualize(nrps);
    }
    void visualize(vector<Natural_Resource_Point>& nrps){
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