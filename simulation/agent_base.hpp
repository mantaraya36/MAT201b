#ifndef INCLUDE_AGENT_BASE_HPP
#define INCLUDE_AGENT_BASE_HPP

#include "allocore/io/al_App.hpp"

float boundary_radius = 90.0f;

struct Agent{
    Vec3f velocity, acceleration;
    Pose pose;
    Color c;
    Quatd q;
    Mesh body;
    float maxspeed;
    float minspeed;
    float mass;
    float initialRadius;
    float maxAcceleration;
    float maxforce;
    float target_senseRadius;
    float desiredseparation;
    float scaleFactor;
    int bioClock;
    float moneySavings;
    float poetryHoldings;
    Vec3f movingTarget;

    void update(){
        velocity += acceleration;
        if (velocity.mag() > maxspeed){
            velocity.normalize(maxspeed);
        }
        pose.pos() += velocity;
        acceleration *= 0; //zeros acceleration
    }    
    void applyForce(Vec3f force){
      Vec3f f = force / mass;
      acceleration += f;
      if (acceleration.mag() > maxAcceleration){
        acceleration.normalize(maxAcceleration);
      }
    }

    void facingToward(Vec3f& target){
        //change facing direction based on target
        Vec3f src = Vec3f(pose.quat().toVectorZ()).normalize();
        Vec3f dst = Vec3f(target - pose.pos()).normalize();
        Quatd rot = Quatd::getRotationTo(src,dst);
        pose.quat() = rot * pose.quat();
    }

    Vec3f seek(Vec3f target){
        Vec3f desired = target - pose.pos();
        desired.normalized();
        desired *= maxspeed;
        Vec3f steer = desired - velocity;
        if (steer.mag() > maxforce){
            steer.normalize(maxforce);
        }
        return steer;
    }

    void inherentDesire(float desireLevel, float radius){
        //inherent desire that changes everyday
        //let them search for something in the metropolis
        bioClock++;
        if (bioClock % 60 == 0) {
            movingTarget = r() * radius;
        }
        if (bioClock > 1440 ){
            bioClock = 0;
        }
        Vec3f skTarget(seek(movingTarget));
        skTarget *= desireLevel;
        applyForce(skTarget);
    }

    //border detect
    void borderDetect(){
        Vec3f origin(0,0,0);
        Vec3f distance = pose.pos() - origin;
        if (distance.mag() > boundary_radius) {
            Vec3f desired = origin - pose.pos();
            Vec3f steer = desired - velocity;
            if (steer.mag() > maxforce) {
                steer.normalize(maxforce);
            }
            applyForce(steer);
        } else {
            applyForce(Vec3f(0,0,0));
        }
    }
};


#endif