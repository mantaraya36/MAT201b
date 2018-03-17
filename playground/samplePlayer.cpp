// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use Gamma's SamplePlayer class to load and play a sound
// - How to render a Waveform with a playback cursor
//

#include "Gamma/Filter.h"
#include "Gamma/Oscillator.h"
#include "Gamma/SamplePlayer.h"
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;
using namespace gam;

// scale the waveform to be this many OpenGL units long
//
#define LENGTH (5.0)

struct AlloApp : App {
  // make a float-based, looping sample player that uses linear interpolation
  // for sample rate conversion.
  //
  SamplePlayer<float, gam::ipl::Linear, phsInc::Loop> player;

  gam::OnePole<> smoothRate;

  Mesh waveform, frame, region, cursor;
  double timer;

  AlloApp() {
    timer = 0;

    smoothRate.freq(3.14159);
    smoothRate = 1.0;

    // find and load a file
    //
    SearchPaths searchPaths;
    searchPaths.addSearchPath("..");
    string filePath = searchPaths.find("diguozhuyi.wav").filepath();
    // string filePath = searchPaths.find("gibber_chimes.wav").filepath();
    cout << filePath << endl;
    player.load(filePath.c_str());

    // build a mesh out of the sound file data
    //
    waveform.primitive(Graphics::LINE_STRIP);
    for (int i = 0; i < player.frames(); i++) {
      waveform.vertex((float)i / player.frames(), player[i]);
      waveform.color(0.1f, 0.5f, 1.0f);
    }

    // build a mesh that's a frame (box) around the waveform
    //
    frame.primitive(Graphics::LINE_LOOP);
    frame.vertex(0, -1);
    frame.vertex(1, -1);
    frame.vertex(1, 1);
    frame.vertex(0, 1);
    frame.color(0, 1, 0);
    frame.color(0, 1, 0);
    frame.color(0, 1, 0);
    frame.color(0, 1, 0);

    // build a mesh that a rectangle that'll show the region being played
    //
    region.primitive(Graphics::TRIANGLES);
    region.vertex(0, -1);
    region.vertex(1, -1);
    region.vertex(1, 1);
    region.vertex(0, 1);
    region.index(0);
    region.index(1);
    region.index(2);
    region.index(0);
    region.index(2);
    region.index(3);
    region.color(1, 0, 0.1, 0.2);
    region.color(1, 0, 0.1, 0.2);
    region.color(1, 0, 0.1, 0.2);
    region.color(1, 0, 0.1, 0.2);

    // a white verticle line to represent the play head
    //
    cursor.primitive(Graphics::LINES);
    cursor.vertex(0, -1);
    cursor.vertex(0, 1);

    // position the camera so we can see the waveform
    //
    nav().pos(2.5, 0, 8);

    initWindow();
    initAudio();
  }

  virtual void onAnimate(double dt) {
    // timer += dt;

    // // periodically trigger a new loop region
    // //
    // if (timer > 5.0) {
    //   timer -= 5.0;

    //   // choose random begin and end points
    //   //
    //   // float begin = rnd::uniform(player.frames());
    //   // float end = rnd::uniform(player.frames());

    //   float begin, end;
    //   for (int t = 0; t < 100; t++) {
    //     begin = rnd::uniform(player.frames());
    //     end = rnd::uniform(player.frames());

    //     if (abs(player[int(begin)] - player[int(end)]) < 0.125) break;
    //   }

    //   // swap if the begining is after the ending
    //   //
    //   if (begin > end) {
    //     float t = begin;
    //     begin = end;
    //     end = t;
    //   }

    //   // tell the player the begin and end points
    //   //
    //   player.min(begin);
    //   player.max(end);

    //   // set playback rate. negative rates play in reverse.
    //   //
    //   float r = pow(2, rnd::uniformS(1.0f));
    //   if (rnd::prob(0.3)) r *= -1;
    //   smoothRate = r;

    //   // start sample from beginning
    //   //
    //   player.reset();
    // }
  }

  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    // turn on transparency and blending
    //
    g.blendModeTrans();
    g.blending(true);

    // render waveform and frame
    //
    g.pushMatrix();
    g.scale(LENGTH, 1, 1);
    g.draw(waveform);
    g.draw(frame);
    g.popMatrix();

    // render cursor (playback head)
    //
    g.pushMatrix();
    g.translate(LENGTH * player.pos() / player.frames(), 0, 0);
    g.draw(cursor);
    g.popMatrix();

    // render the region that's being played
    //
    g.pushMatrix();
    g.translate(LENGTH * player.min() / player.frames(), 0, 0);
    g.scale(LENGTH * (player.max() - player.min()) / player.frames(), 1, 1);
    g.draw(region);
    g.popMatrix();
  }

  virtual void onSound(AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    while (io()) {
      //player.rate(smoothRate());
      float s = player();
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() { AlloApp().start(); }