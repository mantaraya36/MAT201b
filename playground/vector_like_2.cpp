#include <iostream>
#include <vector>
#include <type_traits>
#include "allocore/math/al_Vec.hpp"
#include "allocore/math/al_Random.hpp"

using namespace std;
using namespace al;

#define MAX (1000)

struct State{
    //you cannot use vector
    //vector<Vec3f> stuff;

    //instead do this
    Vec3f stuff[MAX];
    unsigned n;

    void fill_stuff(const vector<Vec3f>& v){
        n = v.size();
        for (unsigned i = 0; i < v.size(); ++i){
            memcpy(&stuff[i], &v[i], sizeof(Vec3f));
            //stuff[i] = v[i];
        }
    }
};

int main(){
    vector<Vec3f> v;
    for (unsigned i = 0; i < 10; i ++){
        v.push_back(Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()));
    }
    State state;
    state.fill_stuff(v);

    for (unsigned i = 0; i < state.n; ++i){
        printf("%f == %f\n", v[i].x, state.stuff[i].x);
    }
    cout << state.stuff << endl;
}