#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

//Mat 201B 2018 Winter
//Mengyu Chen

// some of these must be carefully balanced; i spent some time turning them.
// change them however you like, but make a note of these settings.
unsigned particleCount = 50;     // try 2, 5, 50, and 5000 //500
double maximumAcceleration = 30;  // prevents explosion, loss of particles //30
double initialRadius = 50;        // initial condition //50
double initialSpeed = 5;         // initial condition //50
double gravityFactor = 1e6;       // see Gravitational Constant //1e6
double timeStep = 0.0625;         // keys change this value for effect //0.0625
double scaleFactor = 0.1;         // resizes the entire scene //0.1
double sphereRadius = 6;  // increase this to make collisions more frequent //3
double lifeDecaySpeed = 0.0006; //life decay speed of each particle
double boundary_radius = 120; //a spherical boundary with center at (0,0,0)
Vec3f boundary_origin(0,0,0); //boundary center as (0,0,0)
double spring_k = 1; //k constant best with 0.3 ~ 1
double spring_b = 0.9; //damping coefficiency best with 0 ~ 1

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
  float min_distance;
  Vec3f force;
  Vec3f spring_force;
  Color c;
  Particle() {
    position = r() * initialRadius;
    velocity =
        // this will tend to spin stuff around the y axis
        Vec3f(0, 1, 0).cross(position).normalize(initialSpeed);
    c = HSV(rnd::uniform(), 0.7, 1);
    lifespan = rnd::uniform() * 1.0;
    mass = 1.0;
    min_distance = sphereRadius * 2;
  }
  void applyForce(){
      Vec3f f = acceleration / mass;
      acceleration += f;
      if (acceleration.mag() > maximumAcceleration){
        acceleration.normalize(maximumAcceleration);
      }
      velocity += spring_force; //collision spring effect
  }
  
  bool operator == (const Particle& p) const {
      if (position == p.position && lifespan == p.lifespan){
        return true;
      } else {
          return false;
      }
  }
  bool operator != (const Particle& p) const {
      if (position != p.position || lifespan != p.lifespan){
        return true;
      } else {
          return false;
      }
  }

  void update(){
      velocity += acceleration * timeStep;
      position += velocity * timeStep;
      lifespan -= lifeDecaySpeed;
  }
  void collision_detect(Particle other){
        Vec3f difference = (other.position - position);
        double d = difference.mag();
        //gravitational force     // F = ma where m=1
        force = difference / (d * d * d) * gravityFactor;
        acceleration = force;

        //spring force calculation with hooke's law
        if (d > 0 && d < min_distance){
            Vec3f v = velocity - other.velocity; //relative velocity
            spring_force = -difference * spring_k - v * spring_b;
        } else {
            spring_force.set(0,0,0);
        }
  }   
  void boundary_detect(){
      Vec3f d = position - boundary_origin;
      if (d > boundary_radius) {
          velocity *= -1;
      }
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
        //p.position = origin; //all start from one origin
        particles.push_back(p);
    }
    void applyForce(){
        for (unsigned i = 0; i < particles.size(); ++i){
            for (unsigned j = 1 + i; j < particles.size(); ++j) {
                Particle& a = particles[i];
                Particle& b = particles[j];
                if (a != b){
                    a.collision_detect(b);
                    b.collision_detect(a);
                    a.applyForce();
                    b.applyForce();
                }
            }
        }
    }
    void boundary_detect(){
        for (Particle& p : particles) {
            p.boundary_detect();
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
  bool generator = true;
  ParticleSystem ps;
  float t = 0;
  float generator_speed = 1.5;

  MyApp() {
    //basic settings
    addSphere(sphere, sphereRadius);
    sphere.generateNormals();
    light.pos(0, 0, 0);              // place the light
    nav().pos(0, 0, 30);             // place the viewer
    lens().far(400);                 // set the far clipping plane
    background(Color(0.07));
    initWindow();
    initAudio();

    //description of key functions
    cout << "press 1 : reverse time" << endl;
    cout << "press 2 : speed up time" << endl;
    cout << "press 3 : slow down time" << endl;
    cout << "press 4 : simulate on/off" << endl;
    cout << "press 5 : add one particle" << endl;
    cout << "press 6 : stop auto generating" << endl;
    cout << "press 7 : life decay slower" << endl;
    cout << "press 8 : life decay faster" << endl;
    cout << "press 9 : reset camera position" << endl;
    cout << "press 0 : reset all to default" << endl;
    cout << "press - : slower autogeneration" << endl;
    cout << "press = : faster autogeneration" << endl;
  }

  void onAnimate(double dt) {
    if (!simulate)
      // skip the rest of this function
      return;

    //particle system update
    ps.update();
    ps.applyForce();
    ps.boundary_detect();

    //generator speed control
    if (t < 1) {
        t += dt * generator_speed;
    } else {
        t = 0.0;
    }
    //generate more
    if (generator) {
        if (t == 0.0){
            ps.addParticle();
        }
    }

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
        break;
      case '6':
        generator = !generator;
        break;
      case '7':
        if (lifeDecaySpeed > 0.0001) {
            lifeDecaySpeed /= 2;
        }
        break;
      case '8':
      if (lifeDecaySpeed < 0.05) {
            lifeDecaySpeed *= 2;
        }
        break;
      case '9':
        nav().pos(0, 0, 30);  
        nav().faceToward(Vec3f(0,0,0), 1);
        break; 
      case '=':
        if (generator_speed < 5) generator_speed *= 1.2;
        break;
      case '-':
        if (generator_speed > 0.4) generator_speed /= 1.2;
        break;
      case '0':
        timeStep = 0.0625; 
        simulate = true;
        generator = true;
        lifeDecaySpeed = 0.001;
        generator_speed = 2;
        nav().pos(0, 0, 30);  
        nav().faceToward(Vec3f(0,0,0), 1);
        break;
    }
  }
};

int main() { MyApp().start(); 
}