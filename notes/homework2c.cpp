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
  typedef std::vector<Vec3f> ColorVec;
  ColorVec c_pos;
  ColorVec posA;
  ColorVec posB;
  ColorVec posC;
  ColorVec posD;

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
    c_pos.reserve(w * h);
    posA.reserve(w * h);
    posB.reserve(w * h);
    posC.reserve(w * h);
    posD.reserve(w * h);
    
    //Vec3f m_c_pos[array.height()][array.width()];
    for (size_t row = 0; row < h; ++row) {
      for (size_t col = 0; col < w; ++col) {
        array.read(&pixel, col, row);
        float x = MapValue(col, 0, h, -1, 1);
        float y = MapValue(row, 0, w, -1, 1);
        m_c_pos[row][col].x = x;
        m_c_pos[row][col].y = y;
        m_c_pos[row][col].z = 0;
        //c_pos[row * w + col].set(m_c_pos[row][col]);
        m.vertex(m_c_pos[row][col].x, m_c_pos[row][col].y, m_c_pos[row][col].z);
        m.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
      }
    }
    
    //Vec3f am_c_pos[array.height()][array.width()];
     for (size_t row = 0; row < h; ++row) {
        for (size_t col = 0; col < w; ++col) {
          array.read(&pixel, col, row);
          float x = MapValue(col, 0, array.height(), -1, 1);
          float y = MapValue(row, 0, array.width(), -1, 1);
          am_c_pos[row][col].x = x;
          am_c_pos[row][col].y = y;
          am_c_pos[row][col].z = 0;
          //posA[row * w + col].set(am_c_pos[row][col]);
          am.vertex(am_c_pos[row][col].x, am_c_pos[row][col].y, am_c_pos[row][col].z);
          am.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }
    //Vec3f bm_c_pos[array.height()][array.width()];
      for (size_t row = 0; row < h; ++row) {
        for (size_t col = 0; col < w; ++col) {
          array.read(&pixel, col, row);
          bm_c_pos[row][col].x = MapValue(pixel.r, 0, 255.0, -1, 1);
          bm_c_pos[row][col].y = MapValue(pixel.g, 0, 255.0, -1, 1);
          bm_c_pos[row][col].z = MapValue(pixel.b, 0, 255.0, -1, 1);
          //posB[row * w + col].set(bm_c_pos[row][col]);
          bm.vertex(bm_c_pos[row][col].x, bm_c_pos[row][col].y,bm_c_pos[row][col].z);
          bm.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }

    
    //Vec3f cm_c_pos[array.height()][array.width()];
      for (size_t row = 0; row < h; ++row) {
        for (size_t col = 0; col < w; ++col) {
          array.read(&pixel, col, row);
          hsv = HSV(RGB(pixel.r, pixel.g, pixel.b));
          cm_c_pos[row][col].x = sin(M_PI * 2 * hsv.h) * hsv.s;
          cm_c_pos[row][col].y = cos(M_PI * 2 * hsv.h) * hsv.s;
          cm_c_pos[row][col].z = hsv.v / 255.0;
          //posC[row * w + col].set(cm_c_pos[row][col]);
          cm.vertex( sin(M_PI * 2 * cm_c_pos[row][col].x) * cm_c_pos[row][col].y,  cos(M_PI * 2 * cm_c_pos[row][col].x) * cm_c_pos[row][col].y, cm_c_pos[row][col].z );
          cm.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }

  }
  void onAnimate(double dt){
    
    
    // if (status == 1) {
    //   cout << "stat 1 animating" << endl;
      
    //     for (size_t row = 0; row < h; row++) {
    //       for (size_t col = 0; col < w; col++) {
    //         if (c_pos[row * w + col] != posA[row * w + col] ){
    //           c_pos[row * w + col].lerp(posA[row * w + col], 1 * dt);
    //           m.vertices()[row * w + col].x = c_pos[row * w + col].x;
    //         m.vertices()[row * w + col].y = c_pos[row * w + col].y;
    //         m.vertices()[row * w + col].z = c_pos[row * w + col].z;
    //          }
    //        } 
    //     }
    // }  else if (status == 2){
    //   cout << "stat 2 animating" << endl;
    //   for (size_t row = 0; row < h; row++) {
    //     for (size_t col = 0; col < w; col++) {
    //        if (c_pos[row * w + col] != posB[row * w + col] ){
    //           c_pos[row * w + col].lerp(posB[row * w + col], 1 * dt);
    //           m.vertices()[row * w + col].x = c_pos[row * w + col].x;
    //         m.vertices()[row * w + col].y = c_pos[row * w + col].y;
    //         m.vertices()[row * w + col].z = c_pos[row * w + col].z;
    //          }
    //        } 
    //     }
    
    // } else if (status == 3){
    //   cout << "stat 3 animating" << endl;
    //   for (size_t row = 0; row < h; row++) {
    //     for (size_t col = 0; col < w; col++) {
    //        if (c_pos[row * w + col] != posC[row * w + col] ){
    //           c_pos[row * w + col].lerp(posC[row * w + col], 1 * dt);
    //           m.vertices()[row * w + col].x = c_pos[row * w + col].x;
    //         m.vertices()[row * w + col].y = c_pos[row * w + col].y;
    //         m.vertices()[row * w + col].z = c_pos[row * w + col].z;
    //          }
    //        } 
    //     }
    // }
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
