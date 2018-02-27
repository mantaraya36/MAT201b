#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

// some of these must be carefully balanced; i spent some time turning them.
// change them however you like, but make a note of these settings.
unsigned particleCount = 50;     // try 2, 5, 50, and 5000 //500
double maximumAcceleration = 30;  // prevents explosion, loss of particles //30
double initialRadius = 50;        // initial condition //50
double initialSpeed = 5;         // initial condition //50
double gravityFactor = 1e6;       // see Gravitational Constant //1e6
double timeStep = 0.0625;         // keys change this value for effect //0.0625
double scaleFactor = 0.1;         // resizes the entire scene //0.1
double sphereRadius = 3;  // increase this to make collisions more frequent //3
double lifeDecaySpeed = 0.004; //life decay speed of each particle

Mesh sphere;  // global prototype; leave this alone

// helper function: makes a random vector
Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }

int r_int(int init, int span){
    int v = rand() % span + init;
    return v;
}

struct Particle {
  Vec3f position, velocity, acceleration;
  float lifespan;
  float mass;
  Vec3f force;
  Color c;
  Particle() {
    position = r() * initialRadius;
    velocity =
        // this will tend to spin stuff around the y axis
        Vec3f(0, 1, 0).cross(position).normalize(initialSpeed);
    c = HSV(rnd::uniform(), 0.7, 1);
    lifespan = rnd::uniform() * 1.0;
    mass = 1.0;
  }
  void applyForce(){
      Vec3f f = acceleration / mass;
      acceleration += f;
      if (acceleration.mag() > maximumAcceleration){
        acceleration.normalize(maximumAcceleration);
      }

  }
  
  void update(){
      velocity += acceleration * timeStep;
      position += velocity * timeStep;
      lifespan -= lifeDecaySpeed;
  }
  void detect(Particle other){
      if (other.position != position){
        Vec3f difference = (other.position - position);
        double d = difference.mag();
        //     // F = ma where m=1
        force = difference / (d * d * d) * gravityFactor;
        acceleration = force;
      }
    //     // equal and opposite force (symmetrical)
    //     b.acceleration -= acceleration;
  }   
  void draw(Graphics& g) {
    g.pushMatrix();
    g.translate(position);
    g.color(c);
    g.draw(sphere);
    g.popMatrix();
  }
  bool isDead(){
      if (lifespan < 0.0){
          return true;
      } else {
          return false;
      }
  }
};

struct ParticleSystem {
    vector<Particle> particles;
    Vec3f origin;

    ParticleSystem(){
        origin = Vec3f(0,0,0);
    }
    void addParticle(){
        Particle p;
        //p.position = origin;
        particles.push_back(p);
    }
    void applyForce(){
        for (unsigned i = 0; i < particles.size(); ++i){
            for (unsigned j = 1 + i; j < particles.size(); ++j) {
                Particle& a = particles[i];
                Particle& b = particles[j];
                a.detect(b);
                b.detect(a);
                a.applyForce();
                b.applyForce();
            }
        }
    }
    void update(){
         for (Particle& p : particles) {
            p.update();
        }
    }
    void draw(Graphics& g){
        for (int i = particles.size() - 1; i >= 0; i--){
            Particle& p = particles[i];
            p.draw(g);
            if (p.isDead()){
                particles.erase(particles.begin() + i);
            }
        }
    }
};

struct MyApp : App {
  Material material;
  Light light;
  bool simulate = true;
 
  //vector<Particle> particle;
  ParticleSystem ps;

  MyApp() {
    
    //no need to touch
    addSphere(sphere, sphereRadius);
    sphere.generateNormals();
    light.pos(0, 0, 0);              // place the light
    nav().pos(0, 0, 30);             // place the viewer
    lens().far(400);                 // set the far clipping plane
    background(Color(0.07));
    initWindow();
    initAudio();
    
    //parameters initialize
    
  }

  void onAnimate(double dt) {
    if (!simulate)
      // skip the rest of this function
      return;

    //particle system update
    ps.update();
    ps.applyForce();

    //generate more
    ps.addParticle();


    //
    //  Detect Collisions Here
    //
    
    // for (unsigned i = 0; i < particle.size(); ++i)
    //   for (unsigned j = 1 + i; j < particle.size(); ++j) {
    //     Particle& a = particle[i];
    //     Particle& b = particle[j];
    //     Vec3f difference = (b.position - a.position);
    //     double d = difference.mag();
    //     // F = ma where m=1
    //     Vec3f acceleration = difference / (d * d * d) * gravityFactor;
    //     // equal and opposite force (symmetrical)
    //     a.acceleration += acceleration;
    //     b.acceleration -= acceleration;
    //   }


  }

  void onDraw(Graphics& g) {
    material();
    light();
    g.scale(scaleFactor);
    ps.draw(g);
  }

  void onSound(AudioIO& io) {
    while (io()) {
      io.out(0) = 0;
      io.out(1) = 0;
    }
  }

  void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    switch (k.key()) {
      default:
      case '1':
        // reverse time
        timeStep *= -1;
        break;
      case '2':
        // speed up time
        if (timeStep < 1) timeStep *= 2;
        break;
      case '3':
        // slow down time
        if (timeStep > 0.0005) timeStep /= 2;
        break;
      case '4':
        // pause the simulation
        simulate = !simulate;
        break;
      case '5':
        //generate more particles
        ps.addParticle();
    
    }
  }
};

int main() { MyApp().start(); }