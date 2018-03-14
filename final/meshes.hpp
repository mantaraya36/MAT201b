#ifndef INCLUDE_MESHES_HPP
#define INCLUDE_MESHES_HPP

#include "allocore/io/al_App.hpp"

struct Line : Mesh{
    Line(){
        primitive(Graphics::LINE_STRIP);
        vertex(0,0,0);
        color(0,1,1);

        vertex(0,0,1);
        color(0,1,1);
    }
};
struct Poem : Mesh{
    Poem(){

    }

};


#endif