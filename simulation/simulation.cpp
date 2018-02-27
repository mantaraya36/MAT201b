#include "allocore/io/al_App.hpp"
#include "allocore/math/al_Quat.hpp"
#include "allocore/spatial/al_Pose.hpp"
#include "helper.hpp"
#include "agent_managers.hpp"
#include "agents.hpp"
#include "location_managers.hpp"

using namespace al;
using namespace std;

struct MyApp : App {
    Material material;
    Light light;


    //initial location managers
    Metropolis mps;
    Factories fs;
    NRPs nrps; //manager for Natural Resource Points

    //initial agent managers
    Capitalist_Entity cs;
    Miner_Group ms;


    //event manager
    EventManager eventManager;

    MyApp() {
        light.pos(0, 0, 0);              // place the light
        nav().pos(0, 0, 80);             // place the viewer
        lens().far(400);                 // set the far clipping plane
        background(Color(0.07));
        initWindow();
        initAudio(44100);

        //generate factories according to number of capitalists
        fs.generate(cs);
    }
    void onAnimate(double dt) {

        //locations
        mps.run();
        fs.run();
        nrps.run();

        //agents
        cs.run(mps.mbs);
        ms.run(nrps.nrps, ms.ms);
        
        //eventmanager
        eventManager.updateCollectingStatus(ms.ms);
        nrps.PickEventHandler(eventManager);
        cout << nrps.nrps[0].drained() << " drained?" << endl;
        // cout << nrps.nrps[0].regeneration_rate << " regen rate" << endl;
        cout << nrps.nrps[0].afterDrainTimer << " timer" << endl;
        cout << nrps.nrps[0].resources.size() << "size = count = " << nrps.nrps[0].pickCount << endl;
        //locational behaviors
        fs.drawLinks(cs);
    }
    void onDraw(Graphics& g) {
        material();
        light();
        mps.draw(g);
        fs.draw(g);
        nrps.draw(g);

        cs.draw(g);
        ms.draw(g);
    }
    void onSound(AudioIOData& io) {
        while (io()) {
            io.out(0) = 0;
            io.out(1) = 0;
        }
    }
    void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
        switch(k.key()){
            case '1': fs.drawingLinks = !fs.drawingLinks; break;
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