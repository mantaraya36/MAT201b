#include "allocore/io/al_App.hpp"
#include "allocore/math/al_Quat.hpp"
#include "allocore/spatial/al_Pose.hpp"
using namespace al;
using namespace std;

//MAT 201B Winter 2018
//Mengyu Chen

double scaleFactor = 0.1;  
double coneRadius = 2;
double coneHeight = 6;
double borderRadius = 120;
double boundary_radius = 120; //a spherical boundary with center at (0,0,0)
Vec3f boundary_origin(0,0,0); //boundary center as (0,0,0)
int clan_population_max = 50;
Mesh cone;
Vec3f target(0,0,0);


// helper function: makes a random vector
Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }
//map function
float MapValue(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max  - out_min) / (in_max - in_min) + out_min;
}
struct Frame : Mesh {
        double length = 6;
        Frame(){
            primitive(Graphics::LINES);
            //x axis
            vertex(0, 0, 0);
            color(1,1,1);
            vertex(length, 0, 0);
            color(1, 0, 0);

            //y axis
            vertex(0, 0, 0);
            color(1,1,1);
            vertex(0, length, 0);
            color(0, 1, 0);

            //z axis
            vertex(0, 0, 0);
            color(1,1,1);
            vertex(0, 0, length);
            color(0, 0, 1);
        }
};

struct Boid {
    Vec3f velocity, acceleration;
    Pose pose;
    float maxspeed;
    float maxAcceleration;
    float mass;
    float maxforce;
    float initialRadius; // spawn area
    float senseRadius;
    float desiredseparation;
    Color c;
    Quatd q;
    Frame fm;

    Boid(){
        maxAcceleration = 30;
        mass = 1.0;
        maxspeed = 4;
        maxforce = 0.1;
        initialRadius = 50;
        senseRadius = 60;
        desiredseparation = 10;

        acceleration = Vec3f(0,0,0);
        velocity = Vec3f(0,0,0);
        pose.pos() = r() * initialRadius;
        c = HSV(rnd::uniform(), 0.7, 1);
        
    }
    // double uniqueID(){

    // }
    
    Vec3f seek(Vec3f target){
        Vec3f desired = target - pose.pos();
        desired.normalized();
        desired *= maxspeed;
        Vec3f steer = desired - velocity;
        if (steer.mag() > maxforce){
            steer.normalize(maxforce);
        }
        return steer;
    }

    void arrive(Vec3f target){
        Vec3f desired = target - pose.pos();
        float d = desired.mag();
        desired.normalize();
        if (d < senseRadius){
            float m = MapValue(d, 0, senseRadius, 0, maxspeed);
            desired *= m;
        } else {
            desired *= maxspeed;
        }
        Vec3f steer = desired - velocity;
        if (steer.mag() > maxforce){
            steer.normalize(maxforce);
        }
        applyForce(steer);
    }

    void update(){
        velocity += acceleration;
        if (velocity.mag() > maxspeed){
            velocity.normalize(maxspeed);
        }
        pose.pos() += velocity;
        acceleration *= 0; //zeros acceleration
    }
    Quatd facing_single_target(Vec3f t){
        Quatd q;
        Vec3f src = Vec3f(pose.quat().toVectorZ()).normalize();
        Vec3f dst = Vec3f(t - pose.pos()).normalize();
        Vec3f desired = target - pose.pos();
        float d = desired.mag();
        Quatd rot = Quatd::getRotationTo(src,dst);
        
        if (d < senseRadius){
            q = rot * q;
            return q;
        } else {
            return q;
        }
    }
    void follow_group_direction(vector<Boid> c){
        Vec3f sum(0,0,0);
        int count = 0;
        for (Boid other : c){
            Vec3f difference = pose.pos() - other.pose.pos();
            double d = difference.mag();
            if ((d > 0) && (d < senseRadius)){
                //sum += other.velocity;
                sum += other.pose.pos();
                count ++;
            }
        }
        if (count > 0){
            sum /= count;
            sum = sum + velocity * 10; //position + velocity = temporary target to look at
        } 
        Vec3f src = Vec3f(pose.quat().toVectorZ()).normalize();
        Vec3f dst = Vec3f(sum - pose.pos()).normalize();
        Quatd rot = Quatd::getRotationTo(src,dst);
        pose.quat() =  rot * pose.quat();
        //pose.faceToward(velocity.cross(pose.pos()),1);
    }

    void applyForce(Vec3f force){
      Vec3f f = force / mass;
      acceleration += f;
      if (acceleration.mag() > maxAcceleration){
        acceleration.normalize(maxAcceleration);
      }
     
    }

    Vec3f separate(vector<Boid> c){
        Vec3f sum;
        int count = 0;
        for (Boid other : c){
            Vec3f difference = pose.pos() - other.pose.pos();
            //do we need to check if this == other?
            double d = difference.mag();
            if ((d > 0) && (d < desiredseparation)){
                Vec3d diff = difference.normalize();
                sum += diff;
                count++;
            }
        }
        if (count > 0){
            sum /= count;
            sum.mag(maxspeed);
            Vec3f steer = sum - velocity;
            if (steer.mag() > maxforce) {
                steer.normalize(maxforce);
            }
            return steer;
        } else {
            return Vec3f(0,0,0);
        }
    }
    Vec3f alignment(vector<Boid> c){
        Vec3f sum(0,0,0);
        int count = 0;
        for (Boid other : c) {
            Vec3f difference = pose.pos() - other.pose.pos();
            double d = difference.mag();
            if ((d > 0) && (d < senseRadius)){
                sum += other.velocity;
                count ++;
            }
        }
        if (count > 0){
            sum /= c.size();
            sum.mag(maxspeed);
            Vec3f steer = sum - velocity;
            if (steer.mag() > maxforce) {
                steer.normalize(maxforce);
            }
            return steer;
        } else {
            return Vec3f(0,0,0);
        }
    }

    Vec3f cohesion(vector<Boid> c){
        Vec3f sum(0,0,0);
        int count = 0;
        for (Boid other : c){
            Vec3f difference = pose.pos() - other.pose.pos();
            double d = difference.mag();
            if ((d > 0) && (d < senseRadius)){
                sum += other.pose.pos();
                count ++;
            }
        }
        if (count > 0){
            sum /= count;
            return seek(sum);
        } else {
            return Vec3f(0,0,0);
        }
    }
    
    void flock(vector<Boid> c){
        Vec3f sep(separate(c));
        Vec3f ali(alignment(c));
        Vec3f coh(cohesion(c));
        sep *= 1.5;
        ali *= 1.0;
        coh *= 1.0;
        applyForce(sep);
        applyForce(ali);
        applyForce(coh);
    }

    void seekCenter(){
        Vec3f target(0,0,0);
        Vec3f sk(seek(target));
        sk *= 1.0;
        applyForce(sk);
    }

    // void border_detect(){
    //     Vec3f distance = pose.pos();
    //     if (distance.x > borderRadius) {
    //         Vec3f desire; 
    //     }
    // }

    void draw(Graphics& g) {
        g.pushMatrix();
        g.translate(pose.pos());
        //g.rotate(facing_single_target(Vec3f(0,0,0)));
        g.rotate(pose.quat());
        g.color(c);
        g.draw(cone);
        g.draw(fm);
        g.popMatrix();
    }
    
};

struct Clan {
    vector<Boid> boids;

    Clan(){
    }
    Boid operator [](const int index) const {
        return boids[index];
    }
    vector<Boid> operator ()() const{
        return boids;
    }

    void grow(){
        Boid b;
        //b.position = origin; //all start from one origin
        boids.push_back(b);
    }
    void draw(Graphics& g){
        for (int i = boids.size() - 1; i >= 0; i--){
            Boid& b = boids[i];
            b.draw(g);
            // if (b.isDead()){
            //     b.erase(boids.begin() + i);
            // }
        }
    }
    void run(){
         for (Boid& b : boids) {
            b.update();
            b.flock(boids);
            b.seekCenter();
            b.follow_group_direction(boids);
        }
        //  if (boids.size() > 0){
        //      cout << boids[0].pose.quat().x << " x " << boids[0].pose.quat().y << " y " << boids[0].pose.quat().z << " z " << boids[0].pose.quat().w<< endl; 
        //  }
    }
};

// struct FlowField {
//     vector<vector<vector<Vec3f> > > field;
//     int nx, ny, nz;
//     int resolution;
//     FlowField(){
//         resolution = 5;
//         nx = boundary_radius / resolution;
//         ny = boundary_radius / resolution;
//         nz = boundary_radius / resolution;
//         for (int i = 0; i < nx; ++i) {
//             field[i].resize(ny);
//             for (int j = 0; j < ny; ++j)
//                 field[i][j].resize(nz);
//         }
//     }
//     void init(){
//         float xoff = 0;
//         for (int i = 0; i < nx; ++i){
//             float yoff = 0;
//             for (int j = 0; j < ny; ++j){
//                 float zoff = 0;
//                 for (int k = 0; k < nz; ++k){
//                     float theta = MapValue(rnd::uniform(xoff, yoff),0, 1, 0, M_PI);
//                     field[i][j][k] = Vec3f(rnd::uniform(), cos(theta), sin(theta));
//                     zoff += 0.1;
//                 }
//                 yoff += 0.1;
//             }
//             xoff += 0.1;
//         }
//     }
// };



struct MyApp : App {
  Material material;
  Light light;
  bool simulate = true;
  bool grow = true;
  float t = 0;
  float timer_speed = 1.5;

  Clan c;

  MyApp() {
    //basic settings
    addCone(cone, coneRadius, Vec3f(0,0,coneHeight));
    cone.generateNormals();
    light.pos(0, 0, 0);              // place the light
    nav().pos(0, 0, 30);             // place the viewer
    lens().far(400);                 // set the far clipping plane
    background(Color(0.07));
    initWindow();
    initAudio();

    //description of key functions
    cout << "press 1 : grow on/off" << endl;
    

    
  }

  void onAnimate(double dt) {
    if (!simulate)
      // skip the rest of this function
      return;


    //generator speed control
    if (t < 1) {
        t += dt * timer_speed;
    } else {
        t = 0.0;
    }

     if (grow) {
        if (t == 0.0){
            if (c().size() < clan_population_max){
                c.grow();
            }
        }
    }
    c.run();

  }

  void onDraw(Graphics& g) {
    material();
    light();
    g.scale(scaleFactor);
    c.draw(g);
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
        grow = !grow;
        break;
      case '2':
         // pause the simulation
        simulate = !simulate;
        break;
      case '3':
        
        break;
      case '4':
       
        break;
      case '5':
       
        break;
      case '6':
        
        break;
      case '7':
       
        break;
      case '8':
      
        break;
      case '9':
       
        break; 
      case '=':
        
        break;
      case '-':
        
        break;
      case '0':
       
        nav().pos(0, 0, 30);  
        nav().faceToward(Vec3f(0,0,0), 1);
        break;
    }
  }
};

int main() { MyApp().start(); 
}