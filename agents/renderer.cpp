#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/io/al_App.hpp"

#include "common.hpp"

using namespace al;

struct Phasor{  //saw / ramp
    float phase = 0, increment = 0.001;
    float frequency(float hz, float sampleRate){
        increment = hz / sampleRate; //default SR is 44100, blocksize 128
        //approximately 344 hz
    }
    float getNextSample(){
        float returnValue = phase;
        phase += increment;
        if (phase > 1) phase -=1;
        return returnValue;
    }
    //overload
    float operator()(){
        return getNextSample();
    }
};

struct Sawtooch : Phasor{
    float getNextSample(){
        return 2 * Phasor::getNextSample() - 1; //:: is the scope resolution operator
    }
    float operator()(){
        return getNextSample();
    }
};

struct Reverb{
    float effectSample(float sample){
        return 0;
    }
    float operator()(float sample){
        return effectSample(sample);
    }
};

struct MyApp : App {
  //render basics
  Mesh sphere;
  Mesh cone;
  Material material;
  Light light;
  
  //cuttlebone
  State state;
  cuttlebone::Taker<State> taker;

  //audio
  Phasor phasor;
  Sawtooch saw;

  MyApp() {
    addCone(cone, state.coneRadius, Vec3f(0,0,state.coneHeight)); 
    cone.generateNormals();
    addSphere(sphere, state.sphereRadius);
    sphere.generateNormals();
    light.pos(0, 0, 0);              // place the light
    nav().pos(0, 0, 50);             // place the viewer
    lens().far(400);                 // set the far clipping plane
    background(Color(0.07));
    initWindow();
    initAudio(44100);

    //audio
    phasor.frequency(261,44100);
    saw.frequency(35, 44100); //0.2 hz ->5seconds for one round, or faster

  }

  virtual void onAnimate(double dt) { taker.get(state);

  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    material();
    light();
    
    g.scale(state.scaleFactor);
    for (unsigned i = 0; i < state.boid_number; i ++){
    //for (int i = state.particle_number - 1; i >= 0; i--){
        g.pushMatrix();
        g.translate(state.boid_pos[i]);
        //g.rotate(facing_single_target(Vec3f(0,0,0)));
        g.rotate(state.boid_quat[i]);
        g.color(state.boid_color[i]);
        g.draw(cone);
        g.popMatrix();
    }
    for (unsigned i = 0; i < state.target_number; i ++){
        g.pushMatrix();
        g.translate(state.t_pos[i]);
        g.color(state.t_color[i]);
        g.draw(sphere);
        g.popMatrix();
    }
  }
  virtual void onSound(AudioIOData& io) {
     //io(); // means ready yourself for the next sample set
    while (io()) {
        phasor.frequency(10 * state.solitude_level, 44100);
        saw.frequency(state.average_velocity * 20,44100);
        float sample = phasor() * 0.3 + saw() * 0.08;
        io.out(0) = sample;
        io.out(1) = sample;
    }
  }
};


int main() {
  MyApp app;
  app.taker.start();
  app.start();
}
