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

struct Flat_Mesh : Mesh {
    void initValue(const Array& array ){
      Image::RGBAPix<uint8_t> pixel;
      for (size_t row = 0; row < array.height(); row++) {
        for (size_t col = 0; col < array.width(); col++) {
          array.read(&pixel, col, row);
          float x = MapValue(col, 0, array.height(), -1, 1);
          float y = MapValue(row, 0, array.width(), -1, 1);
          vertex(x, y, 0);
          color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }
    }
};

struct RGB_Mesh : Mesh {
    void initValue(const Array& array ) {
      Image::RGBAPix<uint8_t> pixel;
      Vec3f c_pos[array.height()][array.width()];
      for (size_t row = 0; row < array.height(); row++) {
        for (size_t col = 0; col < array.width(); col++) {
          array.read(&pixel, col, row);
          c_pos[row][col].x = pixel.r / 255.0;
          c_pos[row][col].y = pixel.g / 255.0;
          c_pos[row][col].z = pixel.b / 255.0;
          vertex(c_pos[row][col].x - 0.5, c_pos[row][col].y - 0.5,c_pos[row][col].z - 0.5);
          color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }
    }
};

struct HSV_Mesh : Mesh {
    void initValue(const Array& array ) {
      Image::RGBAPix<uint8_t> pixel;
      HSV hsv;
      Vec3f c_pos[array.height()][array.width()];
      for (size_t row = 0; row < array.height(); row++) {
        for (size_t col = 0; col < array.width(); col++) {
          array.read(&pixel, col, row);
          hsv = HSV(RGB(pixel.r, pixel.g, pixel.b));
          c_pos[row][col].x = hsv.h;
          c_pos[row][col].y = hsv.s;
          c_pos[row][col].z = hsv.v / 255.0;
          vertex( sin(M_PI * 2 * c_pos[row][col].x) * c_pos[row][col].y,  cos(M_PI * 2 * c_pos[row][col].x) * c_pos[row][col].y, c_pos[row][col].z );
          color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }
    }
};

//custom map function
float MapValue(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max  - out_min) / (in_max - in_min) + out_min;
}

class MyApp : public App {
public:
  Image image;
  Texture texture;
  Flat_Mesh fm;
  RGB_Mesh rm;
  HSV_Mesh hm;
  Mesh am;
  Mesh bm;
  Mesh cm;
  

  MyApp() {

    const char *filename = "mat201b/color_spaces/double-rainbow.jpg";
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

    for (size_t row = 0; row < array.height(); ++row) {
      for (size_t col = 0; col < array.width(); ++col) {
        // read the pixel at (row, col) and print
        //array.read(&pixel, row, col);
        array.read(&pixel, col, row);
        
        //cout << "image[" << row << "," << col << "]=" <<
        //(int)pixel.r << "," << (int)pixel.g << "," << (int)pixel.b << endl;
      }
    }
    Vec3f am_c_pos[array.height()][array.width()];
     for (size_t row = 0; row < array.height(); row++) {
        for (size_t col = 0; col < array.width(); col++) {
          array.read(&pixel, col, row);
          float x = MapValue(col, 0, array.height(), -1, 1);
          float y = MapValue(row, 0, array.width(), -1, 1);
          //float x = col / 100.0 - 4;
          //float y = row / 100.0 - 5;
          am.vertex(x, y, 0);
          am.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }
    Vec3f bm_c_pos[array.height()][array.width()];
      for (size_t row = 0; row < array.height(); row++) {
        for (size_t col = 0; col < array.width(); col++) {
          array.read(&pixel, col, row);
          bm_c_pos[row][col].x = pixel.r / 255.0;
          bm_c_pos[row][col].y = pixel.g / 255.0;
          bm_c_pos[row][col].z = pixel.b / 255.0;
          bm.vertex(bm_c_pos[row][col].x - 0.5, bm_c_pos[row][col].y - 0.5,bm_c_pos[row][col].z - 0.5);
          bm.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        }
      }
    
    fm.initValue(array);

    


  }
  void onAnimate(double dt){

  }

  void onDraw(Graphics& g) {
    g.draw(fm);
    //g.draw(rm);
    //g.draw(hm);
  }
};

int main() {
  MyApp app;
  app.initWindow(Window::Dim(600, 400), "imageTexture");
  app.start();
}
