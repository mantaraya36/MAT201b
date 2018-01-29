#include "allocore/io/al_App.hpp"
//iostream is secretly included

using namespace al;
using namespace std;

class MyApp : public App {

};

int main() {
    Vec3f a(0, 0, 1), b(0, 1, 0);
    cout << a << endl;
    cout << b << endl;
    cout << a*b << endl;
    cout << a.dot(b) << "// .dot()" << endl;
    cout << a.cross(b) << "// .cross()" << endl;
    cout << cross(a,b) << "// cross(a,b)" << endl;
    cout << angle(a,b) << "// angle(a,b)" << endl;

    //right hand system, +x to the right, +y to the down, +z facing toward user

    Vec3f c(1,1,1);
    cout << a.dot(c) << endl;
    cout << cross(a,c) << endl;

    //lerp src dest amt
    a.lerp(b, 0.8); // it changes both angle and magnitude
    cout << a << endl;
}