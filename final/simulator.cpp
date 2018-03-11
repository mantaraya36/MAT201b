#include "allocore/io/al_App.hpp"
#include "allocore/math/al_Quat.hpp"
#include "allocore/spatial/al_Pose.hpp"
#include "helper.hpp"
#include "agent_managers.hpp"
#include "location_managers.hpp"

using namespace al;
using namespace std;

struct MyApp : App {
    Material material;
    Light light;

    //initial location managers
    Metropolis metropolis;
    Factories factories;
    NaturalResourcePointsCollection NaturalResourcePts; //manager for Natural Resource Points

    //initial agent managers
    Capitalist_Entity capitalists;
    Miner_Group miners;
    Worker_Union workers;

    //market manager
    MarketManager marketManager;

    MyApp() {
        light.pos(0, 0, 0);              // place the light
        nav().pos(0, 0, 80);             // place the viewer
        lens().far(400);                 // set the far clipping plane
        background(Color(0.4));
        initWindow();
        initAudio(44100);

        //generate factories according to number of capitalists
        factories.generate(capitalists);
        marketManager.statsInit(capitalists, workers, miners);
    }
    void onAnimate(double dt) {
        //market
        marketManager.populationMonitor(capitalists, workers, miners, factories.fs);
        marketManager.updatePrice(capitalists, workers, miners);
        marketManager.capitalMonitor(capitalists, workers, miners, factories.fs);

        //related to market
        factories.getLaborPrice(marketManager);
        miners.calculateResourceUnitPrice(factories.fs);

        //locations
        metropolis.run();
        factories.run(capitalists);
        NaturalResourcePts.run();

        //agents
        capitalists.run(metropolis.mbs);
        miners.run(NaturalResourcePts.nrps, miners.ms, capitalists.cs);
        workers.run(factories.fs, workers.workers, capitalists.cs);
        
        //interaction between groups
        NaturalResourcePts.checkMinerPick(miners.ms);
        factories.checkWorkerNum(workers.workers);
        capitalists.getResource(miners.ms);
        capitalists.getWorkersPaymentStats(factories.fs);

        //pay workers
        factories.payWorkers(marketManager);
       
        //locational behaviors
        factories.drawLinks(capitalists);

        //debug
        // cout << factories.fs[0].materialStocks << "fc material" <<endl;
        // cout << capitalists.cs[0].resourceHoldings << "cp resource" << endl;
        // cout << capitalists.cs[0].resourceClock << "cp clock" <<endl;
        // cout << nrps.nrps[0].drained() << " drained?" << endl;
        // cout << nrps.nrps[0].regeneration_rate << " regen rate" << endl;
        // cout << nrps.nrps[0].afterDrainTimer << " timer" << endl;
        // cout << nrps.nrps[0].resources[0].isPicked << "  r0 is picked?" << endl;
        // cout << nrps.nrps[0].resources[0].beingPicked << "  r0 being picked?" << endl;
        // cout << nrps.nrps[0].pickCount << "  pickcount?" << endl;
        // cout << nrps.nrps[0].resources.size() << "size = count = " << nrps.nrps[0].pickCount << endl;
    }
    void onDraw(Graphics& g) {
        material();
        light();
        //draw all the entities
        metropolis.draw(g);
        factories.draw(g);
        NaturalResourcePts.draw(g);
        capitalists.draw(g);
        miners.draw(g);
         
        workers.draw(g);
    }
    void onSound(AudioIOData& io) {
        while (io()) {
            io.out(0) = 0;
            io.out(1) = 0;
        }
    }
    void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
        switch(k.key()){
            case '1': factories.drawingLinks = !factories.drawingLinks; break;
            case '2': miners.drawingLinks = !miners.drawingLinks; break;
            case '3': workers.drawingLinks = !workers.drawingLinks;break;
            case '4': break;
            case '0': nav().pos(0,0,80);nav().faceToward(Vec3f(0,0,0), 1);
        }
    }

};

int main() { 
    MyApp().start(); 
}