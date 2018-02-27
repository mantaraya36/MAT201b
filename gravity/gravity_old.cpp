#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

Mesh sphere;
//random generating a Vec3f
Vec3f random_V3f(){
    return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
}

struct Particle{
    Vec3f position, velocity, acceleration;
    Color c;
    Particle(){
        position = random_V3f() * 5;
        velocity = random_V3f() * 0.001f;
        c = HSV(rnd::uniform(), 1,1);
    }

    void draw(Graphics& g){
        g.pushMatrix();
        g.translate(position);
        g.color(c);
        g.draw(sphere);
        g.popMatrix();
    }
};

struct MyApp : App {
    double timeStep = 0.01;
    Material material;
    Light light;
    vector<Particle> particles;

    MyApp(){
        nav().pos(0,0,10);
        addSphere(sphere);
        sphere.generateNormals();
        light.pos(5,5,5);
        particles.resize(10);
    }
    double t = 0;
    void onAnimate(double dt){

        
        t += dt;
        //important!!
        //with & it means p is an alias for the particle, not just seperate thing
        //Euler's method
        for (unsigned i = 0; i < particles.size(); ++i) {
            for (unsigned j = 1 + i; j < particles.size(); ++j){
                //for the pair (i,j)
                //create a particle reference "a"
                Particle& a = particles[i];
                Particle& b = particles[j];
                Vec3f direction = (b.position - a.position); // direction is along this vector
                float distance = direction.mag(); //calculate the scalar
                direction.normalize(); //scale it into 1
                Vec3f acceleration = direction / (distance * distance) * timeStep;
                //???
                a.acceleration += acceleration;
                b.acceleration -= acceleration;

                //g * mass1 * mass2 / sqr(r) // distance
            }
        }
        //clamp the force
        float max = 0.01;
        for (auto& p : particles) {
            if (p.acceleration.mag() > max) {
                p.acceleration.normalize(max);
            }
        }

        for (auto& p : particles) {
            p.position += p.velocity;
        }
        for (auto& p : particles) {
            p.velocity += p.acceleration;
        }
    }
    void onDraw(Graphics& g){
        material();
        light();
        for (auto p : particles) {
            p.draw(g);
        }
    }
    void onKeyDown(const Keyboard& k){
		switch(k.key()){
		case '1': timeStep *= -1; break;
		case '2':  break;
		case '3':  break;
		case '4':  break;
        case '5': nav().pos(0,0,4);nav().faceToward(Vec3f(0,0,0), 1);
		}
	}
};

int main(){
    MyApp app;
    app.initWindow(Window::Dim(1024, 768), "pixelTransform");
    app.start();
}