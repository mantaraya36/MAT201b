// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
//
// Shows how to:
// - Use Gamma's RFFT<> class
// - Visualize the FFT with a Mesh
//
#define SOUND_FILE_NAME ("dolphin.wav")

#include <iomanip>
#include "Gamma/DFT.h"
#include "Gamma/SamplePlayer.h"
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;
using namespace gam;

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  searchPaths.print();
  string filePath = searchPaths.find(fileName).filepath();
  if (filePath == "") {
    fprintf(stderr, "ERROR loading file \"\" \n");
    exit(-1);
  }
  return filePath;
}

struct AlloApp : App {
  Mesh m;
  SamplePlayer<float, gam::ipl::Linear, phsInc::Loop> player;

  AlloApp() {
    m.primitive(Graphics::LINE_STRIP);
    player.load(fullPathOrDie(SOUND_FILE_NAME, "..").c_str());

    // decide on a grain size (in samples)
    //
    unsigned grainSize = player.frames() / 100;

    // find the nearest power of 2 larger than that grain size
    //
    unsigned nextPowerOfTwo = pow(2, ceil(log(grainSize) / log(2)));

    // set up the fft object
    //
    RFFT<float> fft(nextPowerOfTwo);

    // allocate a buffer (array of floats)
    //
    float* buffer = new float[fft.size()];

    // initialize all of the buffer to zero
    //
    for (unsigned i = 0; i < fft.size(); i++) buffer[i] = 0;

    // copy the grain data into the buffer (this is where you would apply a
    // window function)
    //
    for (unsigned i = 0; i < grainSize; i++) buffer[i] = player[i + 300];

    // execute the FFT on the buffer
    //
    fft.forward(buffer);

    // calculate how many bins to expect
    //
    unsigned numberOfBins = fft.size() / 2 + 1;
    for (int i = 0; i < numberOfBins; i++) {
      // this is how this FFT implementation lays out the data. i won't explain
      // this here and now. ask me about it sometime.
      //
      gam::Complex<float> c;
      if (i == 0)
        c(buffer[0], 0);
      else if (i == numberOfBins / 2)
        c(buffer[numberOfBins - 1], 0);
      else
        c(buffer[i * 2 - 1], buffer[i * 2]);

      // calculate the magnitude of the current bin
      //
      float magnitude = c.mag();

      // calculate the center frequency of the current bin
      //
      float frequencyOfBin = (float)i / numberOfBins * 44100.0 / 2.0;

      m.vertex(frequencyOfBin, magnitude);

      // print stuff to the terminal
      //
      // cout << scientific << frequencyOfBin << "Hz: " << scientific << c.mag()
      //     << endl;
    }

    initWindow();
    initAudio();
    nav().pos(0, 0, 100);
  }

  virtual void onAnimate(double dt) {}
  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    g.color(1, 1, 1, 1);
    g.draw(m);
  }
  virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k) {}

  virtual void onSound(AudioIOData& io) {
    gam::Sync::master().spu(audioIO().fps());
    // cout << "got here" << endl;
    while (io()) {
      float s = player();
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};

int main() { AlloApp().start(); }