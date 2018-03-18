#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/io/al_App.hpp"
#include "common.hpp"
#include "helper.hpp"
#include "meshes.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
using namespace al;

//Mengyu Chen, 2018
//mengyuchen@ucsb.edu

struct MyApp : OmniStereoGraphicsRenderer {
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

    //needs to be vector, cause everyone is different   
    vector<Mesh> factory_bodies;
    vector<Mesh> factory_bodies_wires;
    
    Mesh metro_body;
    Mesh metro_body_wires;
    int metro_nv;
    Mesh resource_body;
    Mesh resource_body_wires;
    int resource_nv;

    vector<Line> capitalist_lines;
    vector<Line> worker_lines;
    vector<Line> miner_lines;

    Mesh geom;
    ShaderProgram fogshader;
    float phase;

    int renderModeSwitch = 1;

    //cuttlebone
    State state;
    cuttlebone::Taker<State> taker;
    MyApp() {
        light.pos(0, 0, 0);              // place the light
        nav().pos(0, 0, 80);             // place the viewer
        lens().near(0.1).far(150);                     // set the far clipping plane
        //background(Color(0.4));
        initWindow();
        //initAudio(44100);

        //if pure color
        omni().clearColor() = Color(1, 0.85, 0.4, 1.0);

        Mat4f xfm;
        phase = 0;

		for(int i=0; i<1000; ++i){
			xfm.setIdentity();
			xfm.scale(Vec3f(0.2, 0.2, 0.2));
            Vec3f t = r();
            Vec3f temp = t;
			t = t * 60 + temp.normalize(40);
			xfm.translate(t);

			int Nv = addWireBox(geom);
			geom.transform(xfm, geom.vertices().size()-Nv);
		}

        //shader
        phase = 0;
        fogshader.compile(
		R"(
			/* 'fogCurve' determines the distribution of fog between the near and far planes.
			Positive values give more dense fog while negative values give less dense
			fog. A value of	zero results in a linear distribution. */
			uniform float fogCurve;

			/* The fog amount in [0,1] passed to the fragment shader. */
			varying float fogFactor;

			void main(){
				gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
				gl_FrontColor = gl_Color;

				float z = gl_Position.z;
				//float z = gl_FragCoord.z / gl_FragCoord.w; /* per-frament fog would use this */
				fogFactor = (z - gl_Fog.start) * gl_Fog.scale;
				fogFactor = clamp(fogFactor, 0., 1.);
				if(fogCurve != 0.){
					fogFactor = (1. - exp(-fogCurve*fogFactor))/(1. - exp(-fogCurve));
				}
			}
		)",
		R"(
			varying float fogFactor;

			void main(){
				gl_FragColor = mix(gl_Color, gl_Fog.color, fogFactor);
			}
		)"
		);

        //initialize mesh
        //miner body
        miner_nv = addCone(miner_body, MapValue(5000, 0, 100000.0, 1, 3), Vec3f(0,0, MapValue(5000, 0, 100000.0, 1, 3) * 3));
        for(int i=0; i<miner_nv; ++i){
			float f = float(i)/miner_nv;
			miner_body.color(HSV(f*0.1+0.2,1,1));
		}
        miner_body.decompress();
        miner_body.generateNormals();
        addCube(miner_resource, 4);
        miner_resource.generateNormals();

        //worker body
        worker_nv = addCone(worker_body, MapValue(4000, 0, 100000.0, 1, 3), Vec3f(0,0, MapValue(4000, 0, 100000.0, 1, 3) * 3));
        for(int i=0; i<worker_nv; ++i){
			float f = float(i)/worker_nv;
			worker_body.color(HSV(f*0.2+0.4,1,1));
		}
        worker_body.decompress();
        worker_body.generateNormals();

        //capitalist body
        capitalist_nv = addTetrahedron(capitalist_body, MapValue(25000, 0, 100000.0, 3, 6));
        for(int i=0; i<capitalist_nv; ++i){
			float f = float(i)/capitalist_nv;
			capitalist_body.color(HSV(f*0.1,0.9,1));
		}
        capitalist_body.decompress();
        capitalist_body.generateNormals();

        //metro block
        metro_nv = addCube(metro_body);
        addCube(metro_body_wires);
        metro_body_wires.primitive(Graphics::LINE_LOOP);
        for(int i=0; i<metro_nv; ++i){
			float f = float(i)/metro_nv;
			metro_body.color(HSV(0.06 + f*0.1,0.8,1));
            metro_body_wires.color(HSV(0.06 + f*0.1,0.8,1));
		}
        metro_body.decompress();
        metro_body.generateNormals();

        //factory shape
        factory_bodies.resize(state.numCapitalists);
        factory_bodies_wires.resize(state.numCapitalists);
        for (int i = 0; i < state.numCapitalists; i ++){
            addTorus(factory_bodies[i], 0.2, 1.6, r_int(3,6), r_int(3,6));
            addTorus(factory_bodies_wires[i], 0.2 * 1.2, 1.6 * 1.2,r_int(3,6), r_int(3,6) );
            factory_bodies_wires[i].primitive(Graphics::LINE_LOOP);
            factory_bodies[i].generateNormals();
        }

        //resource points
        resource_nv = addDodecahedron(resource_body, 0.3);
        addDodecahedron(resource_body_wires, 0.3 * 1.2);
        resource_body_wires.primitive(Graphics::LINE_LOOP);
        for(int i=0; i<resource_nv; ++i){
			float f = float(i)/resource_nv;
			resource_body.color(HSV(f*0.1,1,1));
            resource_body_wires.color(HSV(f*0.1,1,1));
		}
        resource_body.decompress();
        resource_body.generateNormals();

        //lines
        capitalist_lines.resize(15);
        worker_lines.resize(75);
        miner_lines.resize(100);

    }
   void onAnimate(double dt) { 
        static bool hasNeverHeardFromSim = true;
        if (taker.get(state) > 0){
            hasNeverHeardFromSim = false;
        }
        if (hasNeverHeardFromSim){
            return;
        }
        
        //cout << "heard?" << endl;
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
        for (int i = 0; i < state.numMiners; i ++){
            miner_lines[i].vertices()[0] = state.miner_lines_posA[i];
            miner_lines[i].vertices()[1] = state.miner_lines_posB[i];
        }
        nav().set(state.nav_pose);
        pose = nav();
        phase = state.phase;
        renderModeSwitch = state.renderModeSwitch;

    }
    void onDraw(Graphics& g) {
        if (renderModeSwitch == 1){
            g.blendSub();
        } else if (renderModeSwitch == 2){
            g.blendAdd();
        } else {
            g.blending(false);
        }
        //g.fog(lens().far(), lens().near()+2, HSV(0.1, 0.5, 0.5));
        //fogshader.begin();
        //fogshader.uniform("fogCurve", 4*cos(8*phase*6.2832));
        shader().uniform("lighting", 0.1);
        
        //material();
        //light();
        //draw miners
        for (unsigned i = 0; i < state.numMiners; i ++){
            if (!state.miner_bankrupted[i]){
                g.pushMatrix();
                g.translate(state.miner_pose[i].pos());
                g.rotate(state.miner_pose[i].quat());
                g.scale(state.miner_scale[i]);
                g.draw(miner_body);
                g.popMatrix();
                g.color(0.6,1,0.6);
                g.draw(miner_lines[i]);
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
                g.color(0.4,0.65,1);
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
                g.color(1,0.55,0.4);
                g.draw(capitalist_lines[i]);
            }
            //factories
            g.pushMatrix();
            g.translate(state.factory_pos[i]);
            g.rotate(state.factory_facing_center[i]);
            g.rotate(state.factory_rotation_angle[i],0,0,1);
            g.scale(state.factory_size[i]);
            g.color(state.factory_color[i]);
            g.draw(factory_bodies[i]);
            g.draw(factory_bodies_wires[i]);
            g.popMatrix();
            //metropolis
            g.pushMatrix();
            g.rotate(state.metro_rotate_angle);
                g.pushMatrix();
                g.translate(state.building_pos[i]);
                g.scale(state.building_size[i]);
                g.scale(state.building_size[i], state.building_size[i], state.building_scaleZ[i]);
                g.draw(metro_body);
                g.scale(1.2);
                g.draw(metro_body_wires);
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
                g.draw(resource_body_wires);
                g.popMatrix();
            }
        }
        g.draw(geom);
        //fogshader.end();

    }
    
};

int main() {
  MyApp app;
  app.taker.start();
  app.start();
}