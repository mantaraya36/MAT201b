#include "allocore/io/al_App.hpp"
//iostream is secretly included

using namespace al;
using namespace std;
//euler method
Vec3f position, velocity, acceleration; 

position += velocity * dt;
velocity += acceleration;
//all of these are Vec3f
//each of them can have different direction, depending on each of their xyz values

//**important**
//Euler method concept:
//once you have an acceleration, you accumulate acceleration, then you have velocity
//once you have a velocity, you accumulate velocity, then you have position.
//step by step drifting along curves. (cons of euler method)

//in real world:
//velocity is the dereivative of position (rate of change of position)
//acceleration is the derivitive of velocity
//jerk is the derivitive of acceleration

//in computer simulations
//acceleration comes from made up forces
//velocity comes from integrating acceleration starting as some initial velocity
//position comes from integrating velocity starting as some initial position

//example parabolic equation
//Xt = -5(t-5)2 + 3(t-5) + 8
//Vt = -10(t-5) + 3
//At = -10 //At is constant that it doesn't have t

//F = ma usually, m = 1, so F = a
//Gravitation F = G m1*m2/sqr(r)
//Coulombs law F = Ke q1*q2 / sqr(r)
//Hook's law F = -kX(X1 -X2)
//(simplified) Drag Fd * alpha - v  or   Fd alpha - sqr(v)
//stam's stable fluids

//Particle:
//position, passive, mote, ghosts
//Agents:
//pose, active, body, collides

//Drag
//for (auto& p : particle){
//     p.acceleration += p.velocity * -dragFactor; // dragfactor can be 0.001
// }

// for (auto& p : particle){
//     p.acceleration += p.velocity * p.velocity.mag() * -dragFactor
// } //multiply this magnitude to amplify

