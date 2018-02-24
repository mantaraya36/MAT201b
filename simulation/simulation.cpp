#include "allocore/io/al_App.hpp"
#include "allocore/math/al_Quat.hpp"
#include "allocore/spatial/al_Pose.hpp"
#include "helper.hpp"
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

    MyApp() {
        light.pos(0, 0, 0);              // place the light
        nav().pos(0, 0, 90);             // place the viewer
        lens().far(400);                 // set the far clipping plane
        background(Color(0.07));
        initWindow();
        initAudio(44100);
    }
    void onAnimate(double dt) {
        mps.run();
        fs.run();
        nrps.run();
    }
    void onDraw(Graphics& g) {
        material();
        light();
        mps.draw(g);
        fs.draw(g);
        nrps.draw(g);
    }
    void onSound(AudioIOData& io) {
        while (io()) {
            io.out(0) = 0;
            io.out(1) = 0;
        }
    }
    void onKeyDown(const ViewpointWindow&, const Keyboard& k) {

    }

};

int main() { 
    MyApp().start(); 
}