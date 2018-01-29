#include "allocore/io/al_App.hpp"
//iostream is secretly included

using namespace al;
using namespace std;

struct MyApp : App {

    Color bg;
    Color hsv_c; 
    float h = 0;
    float speed = 0.001;
     
    MyApp (){
        initWindow();
        bg = RGB(1, 0, 0 );
        
        //background(bg);
    }
    void onAnimate(double dt){ // happens before on draw
        h += speed;
        if (h > 1 || h < 0) {
            speed = -speed;
        }
        hsv_c = HSV(h, abs(sin(h)), 1 - h * h);
        background(hsv_c);
    }
    void onDraw(Graphics& g, const Viewpoint& vp){
        
    }
};

int main(){
    MyApp().start();
}