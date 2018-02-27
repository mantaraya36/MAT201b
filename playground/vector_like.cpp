#include <iostream>
#include <allocore/math/al_Vec.hpp>

#define MAX (1000)

struct VectorLikeArray{

    //things can go to cuttlebone
    float data[MAX];
    unsigned size;
    
    void push_back(float f){
        data[size] = f;
        size++;
        if (size >= MAX) printf("WARNING: exceed Simple size\n");
    }
    //cannot go to cuttle bone
    //VectorLikeArray() : size(0) {}

    //instead do this will work
    void init() {
        size = 0;
    }
    //you may also have members

};

void push_back(VectorLikeArray& s, float f){
    s.data[s.size] = f;
    s.size++;
    if (s.size >= MAX) printf("WARNING: exceed Simple size\n");
}


int main(){
    VectorLikeArray simple;
    simple.init();
    push_back(simple,0);
    push_back(simple,1);
    push_back(simple,2);
    push_back(simple,3);
    simple.push_back(4);
    printf("simple.size = %u\n", simple.size);
}