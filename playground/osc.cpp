#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

struct MyApp : App, osc::PacketHandler {
  double t = 0;
  Pose other;
  Mesh mesh;
  Material material;
  Light light;

  void onMessage(osc::Message& m) {
    if (m.addressPattern() == "/xyz") {
      double x,y,z;
      m >> x;
      m >> y;
      m >> z;
      other.pos(Vec3f(x,y,z));
    } else if (m.addressPattern() == "/xyzw"){
      double x,y,z,w;
      m >> x;
      m >> y; 
      m >> z;
      m >> w;
      other.quat() = Quatd(x,y,z,w);
    } else{
      m.print();
    }
  }

  MyApp() {
    initWindow();
    initAudio();
    addCone(mesh, 3, Vec3f(0,0,9));
    mesh.generateNormals();
    light.pos(0, 0, 0);   
    nav().pos(0,0,20);

    //receive from
    char addr[] = "192.168.1.129";
    oscRecv().open(60778, "", 0.016, Socket::UDP);
    oscRecv().handler(*this);
    oscRecv().start();

    //send to
    oscSend().open(60777, "192.168.1.129",0.016, Socket::UDP);
    
  }

  void onAnimate(double dt) {
  
      oscSend().send("/XYZ", nav().pos().x, nav().pos().y, nav().pos().z);
      oscSend().send("/XYZW", nav().quat().x, nav().quat().y, nav().quat().z, nav().quat().w);
      oscSend().send("/Mengyu", "Mengyu");
  }

  void onDraw(Graphics& g) {
    g.pushMatrix();
    g.translate(other.pos());
    g.rotate(other.quat());
    g.color(RGB(0.8,0.3,0.2));
    g.draw(mesh);
    g.popMatrix();

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
        //timeStep *= -1;
        break;
      case '2':
        // speed up time
        //if (timeStep < 1) timeStep *= 2;
        break;
      case '3':
        // slow down time
       // if (timeStep > 0.0005) timeStep /= 2;
        break;
      case '4':
        // pause the simulation
        //simulate = !simulate;
        break;
    }
  }
};

int main() { MyApp().start(); }
