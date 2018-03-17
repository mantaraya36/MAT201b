// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use Gamma's tbl (read Table) function
// - Draw a Mesh with the LINE_STRIP primitive
//
#include "Gamma/Filter.h"
#include "Gamma/DFT.h"
#include "Gamma/Oscillator.h"
#include "Gamma/SamplePlayer.h"
#include "Gamma/tbl.h"
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;
using namespace gam;

struct AlloApp : App {
  Mesh waveform;

  //sample play
  SamplePlayer<float, gam::ipl::Linear, phsInc::Loop> player;
  float timer;

  AlloApp() {
    timer = 0;
    SearchPaths searchPaths;
    searchPaths.addSearchPath("..");
    string filePath = searchPaths.find("gongchandang.wav").filepath();
    player.load(filePath.c_str());

    waveform.primitive(Graphics::POINTS);
    for (int i = 0; i < player.frames(); i+=10) {
      waveform.vertex((float)(i) / player.frames(), player[i], player[i]);
      waveform.color(0.1f, 0.5f, 1.0f);
    }
    
    nav().pos(0.627375, 0.510945, 4.50374);

    initWindow();
    initAudio();
  }

  virtual void onAnimate(double dt) {
      timer ++;
      cout << waveform.vertices().size() << endl;
      for (int i = 0; i < waveform.vertices().size(); i ++){
        waveform.vertices()[i].x = (float)i / player.frames() + sin(i);
        waveform.vertices()[i].y = player[i];
        waveform.vertices()[i].z = (float)i / player.frames() * player[i] + cos(i);
    }

  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    g.draw(waveform);
  }

  virtual void onSound(AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      float s = 0;
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() { AlloApp().start(); }