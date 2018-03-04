#ifndef INCLUDE_LOCATIONS_HPP
#define INCLUDE_LOCATIONS_HPP

#include "allocore/io/al_App.hpp"
#include "helper.hpp"
#include "location_base.hpp"
#include "agents.hpp"

//reference: Platonic Solids by Lance Putnam

using namespace al;
using namespace std;

float MetroRadius = 5.0f;
float FactoryRadius = 25.0f;
float NaturalRadius = 45.0f;
float CirclePadding = 5.0f;

 //forward declaration
struct Worker;
struct Miner;
struct Capitalist;

//interface declaration
struct Resource {
    Vec3f position;
    Color c;
    float angle1;
    float rotation_speed1;
    float angle2;
    float rotation_speed2;
    bool isPicked;
    bool beingPicked;
    int timer;

    Resource(){
        position = r();
        c = HSV(rnd::uniformS(), 0.7, 1);
        angle1 = 0.0f;
        angle2 = 0.0f; 
        rotation_speed1 = rnd::uniform(0.8,1.8);
        rotation_speed2 = rnd::uniform(0.5,1.2);
        isPicked = false;
        beingPicked = false;
        timer = 0;
    }

    void update(){
        angle1 += rotation_speed1;
        if (angle1 > 360){
            angle1 = 0;
        }
        angle2 += rotation_speed2;
        if (angle2 > 360){
            angle2 = 0;
        }
    }
    
};


struct MetroBuilding : Location{
    int mesh_Nv;

    MetroBuilding(){
        scaleFactor = rnd::uniform(1.0,3.0);
        position = r() * MetroRadius;
        mesh_Nv = addCube(mesh);
        for(int i=0; i<mesh_Nv; ++i){
			float f = float(i)/mesh_Nv;
			mesh.color(HSV(f*0.2+0.4,1,1));
		}
        mesh.decompress();
        mesh.generateNormals();
        //c = HSV(rnd::uniform(), 0.7, 1);
    }

    void floating(){
        //floating little bit
    }
    void draw(Graphics& g) {
        g.pushMatrix();
        g.translate(position);
        g.scale(scaleFactor);
        g.color(c);
        g.draw(mesh);
        g.popMatrix();
    }
};

struct Factory : Location{
    float working_radius;
    float meshOuterRadius;
    float meshInnerRadius;
    Vec3f temp_pos;
    float work_timer;
    float work_speed;
    float angle1;
    float angle2;
    float rotation_speed1;
    Quatd q;
    Quatd facing_center;
    Factory(){
        meshOuterRadius = 1.0f;
        meshInnerRadius = 0.2f;
        scaleFactor = rnd::uniform(1.0,2.0);
        position = r();
        temp_pos = position;
        position = position * (FactoryRadius - MetroRadius) + temp_pos.normalize(MetroRadius + CirclePadding);
        addTorus(mesh, meshInnerRadius, meshOuterRadius, r_int(4, 8), r_int(4, 8));
        mesh.generateNormals();
        c = HSV(rnd::uniform(), 0.7, 1);
        work_timer = 0.0f;
        work_speed = 1.0f;
        angle1 = 0.0f;
        angle2 = rnd::uniform(0, 360); // face toward?
        facing_center = Quatd::getRotationTo( Vec3f(q.toVectorZ().normalize()), Vec3f(Vec3f(0,0,0) - position).normalize()) * facing_center;
        rotation_speed1 = rnd::uniform(0.8,2.0);
        working_radius = scaleFactor * meshOuterRadius; //for workers to earn wage
    }
    void initWorkers(){

    }
    void produce(){
        // work_timer += work_speed;
        // if (work_timer > 360.0 ) {
        //     work_speed = 0;
        // }
    }
    void animate(){
        angle1 += rotation_speed1;
        if (angle1 > 360){
            angle1 = 0;
        }
    }

    bool operating(){
        return true;
    }

    void draw(Graphics& g) {
        g.pushMatrix();
        g.translate(position);
        g.rotate(facing_center);
        g.rotate(angle1, 0,0,1);
        g.scale(scaleFactor);
        g.color(c);
        g.draw(mesh);
        g.popMatrix();
    }
};

struct Natural_Resource_Point : Location{
    float meshRadius;
    float resource_spawn_radius;
    int respawn_timer;
    float regeneration_rate;
    Vec3f temp_pos;
    float mesh_Nv;
    float resource_distribution_density;
    int afterDrainTimer;
    int pickCount;
    int maxResourceNum;
    int r_index;
    int initResourceNum;

    vector<Resource> resources;
    vector<bool> drain_check;

    Natural_Resource_Point(){
        scaleFactor = rnd::uniform(1.0,3.0);
        meshRadius = 0.3f;
        resource_distribution_density = 0.4f;
        resource_spawn_radius = meshRadius * (1/resource_distribution_density) * scaleFactor; // for resource to spawn
        position = r();
        temp_pos = position;
        position = position * (NaturalRadius - FactoryRadius) + temp_pos.normalize(FactoryRadius + CirclePadding * 2.0);
        //position = FactoryRadius + CirclePadding + r() * (NaturalRadius - FactoryRadius);
        mesh_Nv = addDodecahedron(mesh, meshRadius);
		for(int i=0; i<mesh_Nv; ++i){
			float f = float(i)/mesh_Nv;
			mesh.color(HSV(f*0.1,1,1));
		}
        mesh.decompress();
        mesh.generateNormals();
        //c = HSV(rnd::uniform(), 0.7, 1);


        //respawn and drain
        respawn_timer = 0;
        regeneration_rate = 0.75f; //based on 60fps, if 1, then every second, if 2, then half a second
        pickCount = 0;
        maxResourceNum = 5;
        r_index = 0;

        //initialize resource
        resources.resize(maxResourceNum);
        for (int i = resources.size() - 1; i >= 0; i--){
            Resource& r = resources[i];
            r.position = position + r.position * resource_spawn_radius;
            r.isPicked = true;
        }
        
        //drain check stuff
        drain_check.resize(maxResourceNum);
        for (int i = drain_check.size() - 1; i >= 0; i--){
            drain_check[i] = true;
        }

        initResourceNum = 5;
        for (int i = 0; i < initResourceNum; i ++){
            resources[i].isPicked = false;
             drain_check[i] = false;
        }
        afterDrainTimer = 0;

    }

    void respawn_resource(){
        if (!drained()){
            respawn_timer++;

            if (respawn_timer % (int)floorf(60.0f / regeneration_rate) == 0){
                int guard_count = 0;
                while (resources[r_index].beingPicked){
                    guard_count ++;
                    if (guard_count > maxResourceNum * 2){
                        cout << "guarded!" << endl;
                        break;
                    }
                    if (r_index < maxResourceNum - 1){
                        r_index += 1;
                    } else if (r_index >= maxResourceNum - 1){
                        r_index = 0;
                    }
                }
                resources[r_index].isPicked = false;
                drain_check[r_index] = false;
                if (r_index < maxResourceNum - 1){
                        r_index += 1;
                } else if (r_index >= maxResourceNum - 1){
                        r_index = 0;
                }
            }

            //don't need to touch here
            if (respawn_timer > 1000) {
                respawn_timer = 0;
            }
            
        } else if (drained()){
            afterDrainTimer ++;
            if (afterDrainTimer == 720){
                int r = r_int(0, maxResourceNum);
                //cout << r << " = index of resources generated" << endl;
                resources[r].isPicked = false;
                drain_check[r] = false;
                afterDrainTimer = 0;
            }
        }
    }

    void update_resource(){
        for (int i = resources.size() - 1; i >= 0; i--){
            Resource& r = resources[i];
            r.update();
            if (r.beingPicked){
                r.timer++;
                if (r.timer == 120){
                    r.isPicked = true;
                    drain_check[i] = true;
                    //r.position.set(200,200,200);
                }
                if (r.timer == 180){
                    r.beingPicked = false;
                    r.timer = 0;
                    // resources.erase(resources.begin() + i);
                }
            }
        }
    }
    bool drained(){
        pickCount = 0;
        for (int i = 0; i < resources.size(); i++){
            if (drain_check[i]){
                pickCount += 1;
            }
        }
        if (pickCount >= resources.size()){
            return true;
        } else {
            return false;
        }
    }

    void draw(Graphics& g) {
        for (int i = resources.size() - 1; i >= 0; i--){
            Resource& r = resources[i];
            if (!r.isPicked){
                g.pushMatrix();
                g.translate(r.position);
                g.rotate(r.angle2, 1,0,0);
                g.rotate(r.angle1, 0,1,0);
                g.scale(scaleFactor);
                g.draw(mesh);
                g.popMatrix();
            }
        }
        
    }
};



#endif