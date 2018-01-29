//hello triangle example

#include "allocore/io/al_App.hpp"
using namespace al;

struct TriApp : App {
    Mesh triangle;

    TriApp(){
        initWindow();
        triangle.vertex(-1, 0, 0);
        triangle.vertex(1,0,0);
        triangle.vertex(0,1,0);
        triangle.primitive(Graphics::TRIANGLES);
        nav().pos(0,0,10); // placing the nav at z axis 10
    }

    void onAnimate(double dt){
        //change the world
        //dt is the amount of time on animation

        //nav().nudgeToward(Vec3f(0,0,-1), 0.01); //??
        nav().faceToward(Vec3f(2,2,2), 0.01);//facing a way litle by little
    
    }
    void onDraw(Graphics& g){
        //rendering the world
        g.draw(triangle);
    }
};

int main() {
    TriApp a;
   a.start();
}