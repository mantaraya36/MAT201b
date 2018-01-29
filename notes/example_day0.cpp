#include "allocore/io/al_App.hpp"
#include <cmath> // pow, sin, sqrt
#include <iostream> //cout, cin
#include <vector>

using namespace al;
//definition of a class
struct MyApp : App {
    Mesh sphere;
    Light light;
    Material material;
    //this is a constructor, which doesn't have a return type
    MyApp(){
    // ^ same name as the class we're defining
    // gets called when a "MyApp" is created

        //make a window
        //use auto to represent a type that 
        //you don't need to take care of 
        // or hold on a variable
        initWindow();
        //auto* w2 = initWindow(); // can have a secondary window

        addSphere(sphere);
        
        sphere.generateNormals();
        light.pos(0,0,10);
        //primitive function takes one argument as:
        //Graphics::LINES openGL way of drawing, check openGL primitives
        sphere.primitive(Graphics::TRIANGLES); 
        
        //add audio to the system
        initAudio();

    
    }

    //this is a destructor
    ~MyApp(){

        //gets called when a MyApp is being destroyed
        printf("MyApp Destroyed\n");
    }

    void onDraw(Graphics& g, const Viewpoint& vp) {
            material();
            light();
            g.draw(sphere);
        }


    //this function will be called for each (audio) frame
    void onSound(AudioIOData& io) {
        io();
        io.out(0) = 1;
        io.out(1) = 1;

        while(io()){
            io.out(0) = 0;
            io.out(1) = 0;
        }
    }
};

//return_type nameOfFunction(argument_type argument_name) {block}
int main(void){
    
    //instantiation
    MyApp app;
    app.start();

    //resizable, generic
    // vector<float> floatList{1,2,3,4,5,6};
    //floatVector.push_back(100.0);
    // print("\n");
    // for(auto e : floatlist) print("%f", e);
}