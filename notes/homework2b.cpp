#include <cassert>
#include <iostream>
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;


// void foo(Array &array) {
//   array.read
  
// };
// void foo(Array *array) {
//   array->read
// } ;

// Array arr;
// foo(arr); ->  foo(Array &array) {};

// foo(&arr) -> void foo(Array *array) {} ;
// foo


//custom map function
float MapValue(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max  - out_min) / (in_max - in_min) + out_min;
}

class MyApp : public App {
public:
  Image image;
  Texture texture;
  Mesh m;
  Mesh am;
  Mesh bm;
  Mesh cm;
  HSV hsv;
  int status = 1;
  int w, h;
  Vec3f m_c_pos[180][279];
  Vec3f am_c_pos[180][279];
  Vec3f bm_c_pos[180][279];
  Vec3f cm_c_pos[180][279];
  //bool animate_done = false;
  //Color bg = RGB(0,0,0);
  
  MyApp() {

    const char *filename = "mat201b/color_spaces/spectrum.jpeg";
    if (image.load(filename)) {
      printf("Read image from %s\n", filename);
    } else {
      printf("Failed to read image from %s!  Quitting.\n", filename);
      exit(-1);
    }
    texture.allocate(image.array());
    Array& array(image.array()); //reference link / creating alias
  
    //print out array 
    cout << "array has " << (int) array.components() << " components" << endl;
    cout << "Array's type (as enum) is " << array.type() << endl;
    printf("Array's type (human readable) is %s\n", allo_type_name(array.type()));
    cout << "Array.print: "  << endl << "   ";
    array.print();    
    assert(array.type() == AlloUInt8Ty);

    Image::RGBAPix<uint8_t> pixel;
    w = array.width();
    h = array.height();

    //m_c_pos[array.height()][array.width()];
    for (size_t row = 0; row < h; ++row) {
      for (size_t col = 0; col < w; ++col) {
        array.read(&pixel, col, row);
        float x = MapValue(col, 0, h, -1, 1);
        float y = MapValue(row, 0, w, -1, 1);
        m_c_pos[row][col].x = x;
        m_c_pos[row][col].y = y;
        m_c_pos[row][col].z = 0;
        m.vertex(m_c_pos[row][col].x, m_c_pos[row][col].y, m_c_pos[row][col].z);
        m.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
      }
    }
    
    //Vec3f am_c_pos[array.height()][array.width()];
     for (size_t row = 0; row < array.height(); ++row) {
        for (size_t col = 0; col < array.width(); ++col) {
          array.read(&pixel, col, row);
          float x = MapValue(col, 0, array.height(), -1, 1);
          float y = MapValue(row, 0, array.width(), -1, 1);
          am_c_pos[row][col].x = x;
          am_c_pos[row][col].y = y;
          am_c_pos[row][col].z = 0;
          am.vertex(am_c_pos[row][col].x, am_c_pos[row][col].y, am_c_pos[row][col].z);
          am.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }
    //Vec3f bm_c_pos[array.height()][array.width()];
      for (size_t row = 0; row < array.height(); ++row) {
        for (size_t col = 0; col < array.width(); ++col) {
          array.read(&pixel, col, row);
          bm_c_pos[row][col].x = MapValue(pixel.r, 0, 255.0, -1, 1);
          bm_c_pos[row][col].y = MapValue(pixel.g, 0, 255.0, -1, 1);
          bm_c_pos[row][col].z = MapValue(pixel.b, 0, 255.0, -1, 1);
          bm.vertex(bm_c_pos[row][col].x, bm_c_pos[row][col].y,bm_c_pos[row][col].z);
          bm.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }

    
      // Vec3f cm_c_pos[array.height()][array.width()];
      for (size_t row = 0; row < array.height(); ++row) {
        for (size_t col = 0; col < array.width(); ++col) {
          array.read(&pixel, col, row);
          hsv = HSV(RGB(pixel.r, pixel.g, pixel.b));
          //cm_c_pos[row][col].x = hsv.h;
          cm_c_pos[row][col].x = sin(M_PI * 2 * hsv.h) * hsv.s;
          //cm_c_pos[row][col].y = hsv.s;
          cm_c_pos[row][col].y = cos(M_PI * 2 * hsv.h) * hsv.s;
          cm_c_pos[row][col].z = hsv.v / 255.0;
          //cm.vertex( sin(M_PI * 2 * cm_c_pos[row][col].x) * cm_c_pos[row][col].y,  cos(M_PI * 2 * cm_c_pos[row][col].x) * cm_c_pos[row][col].y, cm_c_pos[row][col].z );
          //cm.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }

  }
  void onAnimate(double dt){
    
    
    if (status == 1) {
      cout << "stat 1 animating" << endl;
      cout << am_c_pos[1][1] << "am 1 1" << endl;
      
        for (size_t row = 0; row < h; row++) {
          for (size_t col = 0; col < w; col++) {
            if (m_c_pos[row][col] != am_c_pos[row][col] ){
              m_c_pos[row][col].lerp(am_c_pos[row][col], 1 * dt);
              m.vertices()[row * w + col].x = m_c_pos[row][col].x;
            m.vertices()[row * w + col].y = m_c_pos[row][col].y;
            m.vertices()[row * w + col].z = m_c_pos[row][col].z;
             }
           } 
        }
      cout << m_c_pos[1][1] << "m 1 1" << endl;
    }  else if (status == 2){
      cout << "stat 2 animating" << endl;
      cout << bm_c_pos[1][1] << "bm 1 1" << endl;
      for (size_t row = 0; row < h; row++) {
        for (size_t col = 0; col < w; col++) {
          if (m_c_pos[row][col] != bm_c_pos[row][col] ){
            m_c_pos[row][col].lerp(bm_c_pos[row][col], 1 * dt);
            m.vertices()[row * w + col].x = m_c_pos[row][col].x;
            m.vertices()[row * w + col].y = m_c_pos[row][col].y;
            m.vertices()[row * w + col].z = m_c_pos[row][col].z;
           }
        }
      }
       cout << m_c_pos[1][1] << "m 1 1" << endl;
    } else if (status == 3){
      cout << "stat 3 animating" << endl;
      cout << cm_c_pos[1][1] << "cm 1 1" << endl;
      for (size_t row = 0; row < h; row++) {
        for (size_t col = 0; col < w; col++) {
          if (m_c_pos[row][col] != cm_c_pos[row][col] ){
            m_c_pos[row][col].lerp(cm_c_pos[row][col],  1 * dt);
            m.vertices()[row * w + col].x = m_c_pos[row][col].x;
            m.vertices()[row * w + col].y = m_c_pos[row][col].y;
            //m.vertices()[row * w + col].x = sin(M_PI * 2 * m_c_pos[row][col].x) * m_c_pos[row][col].y;
            //m.vertices()[row * w + col].y = cos(M_PI * 2 * m_c_pos[row][col].x) * m_c_pos[row][col].y;
            m.vertices()[row * w + col].z = m_c_pos[row][col].z;
           }
        }
      }
       cout << m_c_pos[1][1] << "m 1 1" << endl;
    }
    //background(bg);
  }

  void onDraw(Graphics& g) {
    g.draw(m);
    //g.draw(rm);
    //g.draw(hm);
  }
  void onKeyDown(const Keyboard& k){
		switch(k.key()){
		case '1': status = 1; break;
		case '2': status = 2;; break;
		case '3': status = 3; break;
		case '4': status = 4; break;
		}
	}
};

int main() {
  MyApp app;
  app.initWindow(Window::Dim(800, 600), "imageTexture");
  app.start();
}
