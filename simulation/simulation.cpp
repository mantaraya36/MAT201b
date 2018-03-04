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
    NRPs NaturalResourcePts; //manager for Natural Resource Points

    //initial agent managers
    Capitalist_Entity capitalists;
    Miner_Group miners;

    //event manager
    //EventManager eventManager;

    MyApp() {
        light.pos(0, 0, 0);              // place the light
        nav().pos(0, 0, 80);             // place the viewer
        lens().far(400);                 // set the far clipping plane
        background(Color(0.4));
        initWindow();
        initAudio(44100);

        //generate factories according to number of capitalists
        factories.generate(capitalists);
    }
    void onAnimate(double dt) {
        //locations
        metropolis.run();
        factories.run();
        NaturalResourcePts.run();

        //agents
        capitalists.run(metropolis.mbs);
        miners.run(NaturalResourcePts.nrps, miners.ms, capitalists.cs);
        
        //eventmanager
        //eventManager.updateCollectingStatus(miners.ms);
        //NaturalResourcePts.PickEventHandler(eventManager);

        //interaction between groups
        NaturalResourcePts.checkMinerPick(miners.ms);
        capitalists.getResource(miners.ms);
       
        //locational behaviors
        factories.drawLinks(capitalists);

        //debug
        //  cout << nrps.nrps[0].drained() << " drained?" << endl;
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
            case '2': break;
            case '3': break;
            case '4': break;
            case '0': nav().pos(0,0,80);nav().faceToward(Vec3f(0,0,0), 1);
        }
    }

};

int main() { 
    MyApp().start(); 
}