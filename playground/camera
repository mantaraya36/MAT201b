#include "allocore/io/al_App.hpp"
//iostream is secretly included

using namespace al;
using namespace std;

struct Frame : Mesh {
    Frame(){
        primitive(Graphics::LINES);
        //x axis
        vertex(0, 0, 0);
        color(1,1,1);
        vertex(1, 0, 0);
        color(1, 0, 0);

        //y axis
        vertex(0, 0, 0);
        color(1,1,1);
        vertex(0, 1, 0);
        color(0, 1, 0);

        //z axis
        vertex(0, 0, 0);
        color(1,1,1);
        vertex(0, 0, 1);
        color(0, 0, 1);
    }
};
struct Weird : Mesh{
    Weird(){
    primitive(Graphics::LINE_STRIP);
        for (int i = 0; i < 100; i ++){
            for (int j = 0; j < 10; j ++){
            vertex(sin(M_PI * 2 * i / 100) * j, cos(M_PI * 2 * i / 100) * j, 0);
            vertex(0, sin(M_PI * 2 * i / 100) * j, cos(M_PI * 2 * i / 100) * j);
            vertex(sin(M_PI * 2 * i / 100) * j, 0, cos(M_PI * 2 * i / 100) * j);
            color(HSV(0.5, 1, 0.5));
            }
        }
    }
};
struct Circle : Mesh {
    Circle(){
        primitive(Graphics::LINE_STRIP);
        for (int i = 0; i < 100; i ++){
            vertex(sin(M_PI * 2 * i / 100), cos(M_PI * 2 * i / 100), 0);
            color(HSV(0.5, 1, 0.5));
            
        }
    }
    }
};

struct MyApp : App {

    Color hsv_c; 
    float h = 0;
    float speed = 0.001;
    Circle c;
    Frame frame;
    float r = 0.5;
     
    MyApp (){
        initWindow();
        hsv_c = HSV(0.5, 1, 0.5);
        //background(hsv_c);

        //use a mesh like an array of vec3f;
        c.vertices()[0].z = 5;
        //const Buffer<Circle::Vertex>
        c.vertices()[60] = cross(c.vertices()[10], c.vertices()[90]);
        
    }
    void onAnimate(double dt){ // happens before on draw
        h += speed;
        if (h > 1 || h < 0) {
            speed = -speed;
        }
    }
    void onDraw(Graphics& g, const Viewpoint& vp){
        g.draw(frame);
        g.draw(c);
    }
};

int main(){
    MyApp().start();
}