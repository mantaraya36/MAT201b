#include <iostream>
using namespace std;

struct Vec3f {
  union {
    //union means everything in the union they take place in the same memory
    //float has 4 bytes, if we don't have union, we have 24 bytes, right now 12 bytes
    struct { float x, y, z; }; // this line is correct
    float elems[3];
    //elems and xyz are unionized. so 24 bytes become 12 bytes
     //elems[0] ==> x 
     //elems[1] ==> y
  };
};

int main() {
  Vec3f foo;
  foo.x = 10;
  foo.y = 20;
  foo.z = 30;
  cout << foo.elems[0] << endl;
  cout << foo.elems[1] << endl;
  cout << foo.elems[2] << endl;
  //output should look like 
// 10
// 20
// 30
}



//dot product
//projection

//cross product
//a vector perpendicular to both

//normalize(mag = 1)
//sets length to 1 if you don't give a particular argument
//make it length as 1, not too long or too short

//magnitude
//length

//lerp
//move in between two vectors with linear interpolation
// {
// Vec2f v(3,4);

// 		v.mag();				// Returns 5, the magnitude of the vector
// 		v.magSqr();				// Returns 25, the magnitude squared

//         //mag ==> length

// 		v.normalize();			// Sets magnitude to 1 without changing the
// 								// vector's direction; the map a <- a / a.mag().

// 		v.normalize(0.5);		// Sets magnitude to 0.5 without changing the
// 								// direction

// 		v.normalized();			// Returns closest vector on unit sphere
// }