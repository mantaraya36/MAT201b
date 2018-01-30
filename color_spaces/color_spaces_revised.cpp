#include <cassert>
#include <iostream>
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

//map function
float MapValue(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max  - out_min) / (in_max - in_min) + out_min;
}

class MyApp : public App {
public:
  Image image;
  Texture texture;
  Mesh m;
  HSV hsv;
  int status = 1;
  int w, h;
  std::vector<Vec3f> c_pos;
  std::vector<Vec3f> posA;
  std::vector<Vec3f> posB;
  std::vector<Vec3f> posC;
  std::vector<Vec3f> posD;
  float t = 0;
  
  MyApp() {
    nav().pos(0,0,2);
    const char *filename = "mat201b/color_spaces/plastic_city.jpg";
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

    for (size_t row = 0; row < h; ++row) {
      for (size_t col = 0; col < w; ++col) {
        array.read(&pixel, col, row);
        float x = MapValue(col, 0, h, -1, 1);
        float y = MapValue(row, 0, w, -1, 1);
        posA[row * w + col].x = x;
        posA[row * w + col].y = y;
        posA[row * w + col].z = 0;
        posB[row * w + col].x = MapValue(pixel.r, 0, 255.0, -1, 1);
        posB[row * w + col].y = MapValue(pixel.g, 0, 255.0, -1, 1);
        posB[row * w + col].z = MapValue(pixel.b, 0, 255.0, -1, 1);
        hsv = HSV(RGB(pixel.r, pixel.g, pixel.b));
        posC[row * w + col].x = sin(M_PI * 2 * hsv.h) * hsv.s;
        posC[row * w + col].y = cos(M_PI * 2 * hsv.h) * hsv.s;
        posC[row * w + col].z = hsv.v / 255.0;
        posD[row * w + col].x = sin(M_PI * 2 * hsv.s) * sin(M_PI * 2 * hsv.h) * hsv.v / 255.0;
        posD[row * w + col].y = cos(M_PI * 2 * hsv.s) * sin(M_PI * 2 * hsv.h) * hsv.v / 255.0;
        posD[row * w + col].z = cos(M_PI * 2 * hsv.h) * hsv.v / 255.0;

        c_pos[row * w + col].set(posA[row * w + col]);
        m.vertex(c_pos[row * w + col].x, c_pos[row * w + col].y, c_pos[row * w + col].z);
        m.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
      }
    }
  }
  void onAnimate(double dt){
  if (t < 1.0) t += dt / 10;
  
  if (status == 1){
    for (unsigned i = 0; i < m.vertices().size(); i++){
        c_pos[i].lerp(posA[i], t);
        m.vertices()[i].x = c_pos[i].x;
        m.vertices()[i].y = c_pos[i].y;
        m.vertices()[i].z = c_pos[i].z;
    }
  } else if (status == 2){
      for (unsigned i = 0; i < m.vertices().size(); i++){
          c_pos[i].lerp(posB[i], t);
          m.vertices()[i].x = c_pos[i].x;
          m.vertices()[i].y = c_pos[i].y;
          m.vertices()[i].z = c_pos[i].z;
    }
  }else if (status == 3){
      for (unsigned i = 0; i < m.vertices().size(); i++){
          c_pos[i].lerp(posC[i], t);
          m.vertices()[i].x = c_pos[i].x;
          m.vertices()[i].y = c_pos[i].y;
          m.vertices()[i].z = c_pos[i].z;
    }
  }  else if (status == 4){
      for (unsigned i = 0; i < m.vertices().size(); i++){
          c_pos[i].lerp(posD[i], t);
          m.vertices()[i].x = c_pos[i].x;
          m.vertices()[i].y = c_pos[i].y;
          m.vertices()[i].z = c_pos[i].z;
    }
  }
  
  lens().fovx(30 + 60 * (sin(t * 10)+1), (sin(t * 10)+ 1)* 2);
  
  }

  void onDraw(Graphics& g) {
    g.draw(m);
  }
  void onKeyDown(const Keyboard& k){
		switch(k.key()){
		case '1': status = 1; t = 0;; break;
		case '2': status = 2; t = 0; break;
		case '3': status = 3; t = 0; break;
		case '4': status = 4; t = 0; break;
    case '5': nav().pos(0,0,4);nav().faceToward(Vec3f(0,0,0), 1);
		}
	}
};

int main() {
  MyApp app;
  app.initWindow(Window::Dim(1024, 768), "pixelTransform");
  app.start();
}
