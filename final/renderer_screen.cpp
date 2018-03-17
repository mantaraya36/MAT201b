#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/io/al_App.hpp"
#include "common.hpp"
#include "helper.hpp"
#include "meshes.hpp"
//#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
using namespace al;

//Mengyu Chen, 2018
//mengyuchen@ucsb.edu

struct MyApp : App {
    //basics
    Material material;
    Light light;

    //meshes
    Mesh miner_body;
    Mesh miner_resource;
    int miner_nv;
    Mesh worker_body;
    int worker_nv;
    Mesh capitalist_body;
    int capitalist_nv;
    Mesh factory_body;
    Mesh metro_body;
    int metro_nv;
    Mesh resource_body;
    int resource_nv;
    vector<Line> capitalist_lines;
    vector<Line> worker_lines;

    //cuttlebone
    State state;
    cuttlebone::Taker<State> taker;
    MyApp() {
        light.pos(0, 0, 0);              // place the light
        nav().pos(0, 0, 80);             // place the viewer
        lens().far(400);                 // set the far clipping plane
        background(Color(0.4));
        initWindow();
        //initAudio(44100);

        //initialize mesh
        //miner body
        miner_nv = addCone(miner_body, MapValue(5000, 0, 100000.0, 1, 3), Vec3f(0,0, MapValue(5000, 0, 100000.0, 1, 3) * 3));
        for(int i=0; i<miner_nv; ++i){
			float f = float(i)/miner_nv;
			miner_body.color(HSV(f*0.05,0.9,1));
		}
        miner_body.decompress();
        miner_body.generateNormals();
        addCube(miner_resource, 4);
        miner_resource.generateNormals();

        //worker body
        worker_nv = addCone(worker_body, MapValue(4000, 0, 100000.0, 1, 3), Vec3f(0,0, MapValue(4000, 0, 100000.0, 1, 3) * 3));
        for(int i=0; i<worker_nv; ++i){
			float f = float(i)/worker_nv;
			worker_body.color(HSV(f*0.65,0.9,1));
		}
        worker_body.decompress();
        worker_body.generateNormals();

        //capitalist body
        capitalist_nv = addCone(capitalist_body, MapValue(25000, 0, 100000.0, 3, 6), Vec3f(0,0, MapValue(25000, 0, 100000.0, 3, 6) * 3));
        for(int i=0; i<capitalist_nv; ++i){
			float f = float(i)/capitalist_nv;
			capitalist_body.color(HSV(f*0.2,0.9,1));
		}
        capitalist_body.decompress();
        capitalist_body.generateNormals();

        //metro block
        metro_nv = addCube(metro_body);
        for(int i=0; i<metro_nv; ++i){
			float f = float(i)/metro_nv;
			metro_body.color(HSV(0.06 + f*0.1,0.8,1));
		}
        metro_body.decompress();
        metro_body.generateNormals();

        //factory shape
        addTorus(factory_body, 0.2, 1.6, r_int(3,6), r_int(3,6));
        factory_body.generateNormals();

        //resource points
        resource_nv = addDodecahedron(resource_body, 0.3);
        for(int i=0; i<resource_nv; ++i){
			float f = float(i)/resource_nv;
			resource_body.color(HSV(f*0.1,1,1));
		}
        resource_body.decompress();
        resource_body.generateNormals();

        //lines
        capitalist_lines.resize(20);
        worker_lines.resize(100);

    }
    virtual void onAnimate(double dt) { 
        taker.get(state);
        // capitalist_lines.resize(state.numCapitalists);
        // worker_lines.resize(state.numWorkers);
        for (int i = 0; i < state.numCapitalists; i ++){
            capitalist_lines[i].vertices()[0] = state.capitalist_lines_posA[i];
            capitalist_lines[i].vertices()[1] = state.capitalist_lines_posB[i];
        }
        for (int i = 0; i < state.numWorkers; i ++){
            worker_lines[i].vertices()[0] = state.worker_lines_posA[i];
            worker_lines[i].vertices()[1] = state.worker_lines_posB[i];
        }
        nav().set(state.nav_pose);

    }
    virtual void onDraw(Graphics& g, const Viewpoint& v) {
        material();
        light();
        //shader().uniform("lighting", 1.0);
        g.blendAdd();

        //draw miners
        for (unsigned i = 0; i < state.numMiners; i ++){
            if (!state.miner_bankrupted[i]){
                g.pushMatrix();
                g.translate(state.miner_pose[i].pos());
                g.rotate(state.miner_pose[i].quat());
                g.scale(state.miner_scale[i]);
                g.draw(miner_body);
                g.popMatrix();
            }
        }
        //draw workers
        for (unsigned i = 0; i < state.numWorkers; i ++){
            if (!state.worker_bankrupted[i]){
                g.pushMatrix();
                g.translate(state.worker_pose[i].pos());
                g.rotate(state.worker_pose[i].quat());
                g.scale(state.worker_scale[i]);
                g.draw(worker_body);
                g.popMatrix();
                //lines
                g.draw(worker_lines[i]);
            }
        }
        //draw capitalists, factories, and buildings
        for (unsigned i = 0; i < state.numCapitalists; i ++){
            //capitalists
            if (!state.capitalist_bankrupted[i]){
                g.pushMatrix();
                g.translate(state.capitalist_pose[i].pos());
                g.rotate(state.capitalist_pose[i].quat());
                g.scale(state.capitalist_scale[i]);
                g.draw(capitalist_body);
                g.popMatrix();
                //lines
                g.draw(capitalist_lines[i]);
            }
            //factories
            g.pushMatrix();
            g.translate(state.factory_pos[i]);
            g.rotate(state.factory_facing_center[i]);
            g.rotate(state.factory_rotation_angle[i],0,0,1);
            g.scale(state.factory_size[i]);
            g.color(state.factory_color[i]);
            g.draw(factory_body);
            g.popMatrix();
            //metropolis
            g.pushMatrix();
            g.rotate(state.metro_rotate_angle);
                g.pushMatrix();
                g.translate(state.building_pos[i]);
                g.scale(state.building_size[i]);
                g.scale(state.building_size[i], state.building_size[i], state.building_scaleZ[i]);
                g.draw(metro_body);
                g.popMatrix();
            g.popMatrix();
        }
        
        //resource
        for (int i = 0; i < state.numResources; i ++){
            if (!state.resource_picked[i]){
                g.pushMatrix();
                g.translate(state.resource_pos[i]);
                g.rotate(state.resource_angleA[i], 0, 1, 0);
                g.rotate(state.resource_angleB[i], 1, 0, 0);
                g.scale(state.resource_scale[i]);
                g.draw(resource_body);
                g.popMatrix();
            }
        }

    }
    
    virtual void onSound(AudioIOData& io) {
     //io(); // means ready yourself for the next sample set
        while (io()) {
            io.out(0) = 0;
            io.out(1) = 0;
        }
    }
};

int main() {
  MyApp app;
  app.taker.start();
  app.start();
}