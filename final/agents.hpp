#ifndef INCLUDE_AGENTS_HPP
#define INCLUDE_AGENTS_HPP

#include "allocore/io/al_App.hpp"
#include "allocore/math/al_Quat.hpp"
#include "allocore/spatial/al_Pose.hpp"
#include "helper.hpp"
#include "agent_base.hpp"
#include "locations.hpp"

using namespace al;
using namespace std;

// struct Resource;
// struct Factory;
// struct Natural_Resource_Point;
// struct MetroBuilding;
struct Capitalist : Agent {
    int mesh_Nv;
    Vec3f movingTarget;
    int desireChangeRate;
    int resourceHoldings;
    int TimeToDistribute;
    int resourceClock;

    Capitalist(){
        //initial params
        maxAcceleration = 2;
        mass = 1.0;
        maxspeed = 1;
        minspeed = 0.3;
        maxforce = 0.1;
        initialRadius = 5;
        target_senseRadius = 10.0;
        desiredseparation = 3.0f;
        acceleration = Vec3f(0,0,0);
        velocity = Vec3f(0,0,0);
        pose.pos() = r() * initialRadius;
        bioClock = 0;
        movingTarget = r();
        desireChangeRate = r_int(50, 150);

        //capitals
        resourceHoldings = r_int(5, 25);
        capitalHoldings = r_int(30, 90);
        poetryHoldings = 0.0;

        //factory relation
        TimeToDistribute = 720;

        //draw body
        scaleFactor = 0.3; //richness?
        mesh_Nv = addCone(body, capitalHoldings / 30.0, Vec3f(0,0,capitalHoldings / 30.0 * 3));
        for(int i=0; i<mesh_Nv; ++i){
			float f = float(i)/mesh_Nv;
			body.color(HSV(f*0.2,0.9,1));
		}
        body.decompress();
        body.generateNormals();
    }
    void run(vector<MetroBuilding>& mbs){
        
        
        //basic behaviors
        Vec3f ahb(avoidHittingBuilding(mbs));
        ahb *= 0.8;
        applyForce(ahb);

        //factory related
        distributeResources();

        //default behaviors
        borderDetect();
        inherentDesire(0.5, 0, MetroRadius, desireChangeRate);
        facingToward(movingTarget);
        update();
    }
    void distributeResources(){
        resourceClock ++;
        //every 12 seconds, half a day, distribute resource
        if (resourceClock == TimeToDistribute) {
            resourceHoldings = 0;
            resourceClock = 0;
        }
    }

    void learnPoems(){

    }
    bool bankrupted(){
        return false;
    }

    Vec3f avoidHittingBuilding(vector<MetroBuilding>& mbs){
        Vec3f sum;
        int count = 0;
        for (MetroBuilding mb : mbs){
            Vec3f difference = pose.pos() - mb.position;
            float d = difference.mag();
            if ((d > 0) && (d < desiredseparation)){
                Vec3d diff = difference.normalize();
                sum += diff;
                count++;
            }
        }
        if (count > 0){
            sum /= count;
            sum.mag(maxspeed);
            Vec3f steer = sum - velocity;
            if (steer.mag() > maxforce) {
                steer.normalize(maxforce);
            }
            return steer;
        } else {
            return Vec3f(0,0,0);
        }
    }

    void draw(Graphics& g){
        g.pushMatrix();
        g.translate(pose.pos());
        g.rotate(pose.quat());
        g.scale(scaleFactor);
        g.draw(body);
        g.popMatrix();
    }
};


struct Miner : Agent {
    int mesh_Nv;
    Vec3f movingTarget;
    Vec3f temp_pos;

    bool resourcePointFound;
    float distToClosestNRP;
    float distToClosestResource;
    int id_ClosestNRP;
    int id_ClosestResource;
    float searchResourceForce;
    float collectResourceForce;
    float sensitivityNRP;
    float sensitivityResource;
    float pickingRange;
    float sensitivityCapitalist;
    float desireLevel;
    float separateForce;
    float resourceHoldings;
    int collectTimer;
    int tradeTimer;
    float distToClosestCapitalist;
    int id_ClosestCapitalist;
    bool capitalistNearby;
    int desireChangeRate;
    float businessDistance;
    int unloadTimeCost;
    float collectRate;
    int maxLoad;
    Mesh resource;

    Miner(){
        maxAcceleration = 1;
        mass = 1.0;
        maxspeed = 0.3;
        minspeed = 0.1;
        maxforce = 0.03;
        initialRadius = 5;
        target_senseRadius = 10.0;
        desiredseparation = 3.0f;
        acceleration = Vec3f(0,0,0);
        velocity = Vec3f(0,0,0);
        pose.pos() = r();
        temp_pos = pose.pos();
        pose.pos() = pose.pos() * (NaturalRadius - FactoryRadius) + temp_pos.normalize(FactoryRadius + CirclePadding * 2.0);
        bioClock = 0;
        movingTarget = r();
        desireChangeRate = r_int(60, 90);

        //relation to resource point
        distToClosestNRP = 120.0f;
        distToClosestResource = 120.0f;
        id_ClosestNRP = 0;
        id_ClosestResource = 0;
        resourcePointFound = false;
        searchResourceForce = 1.0;
        collectResourceForce = 1.0;
        sensitivityNRP = 30.0;
        sensitivityResource = 8.0;
        pickingRange = 2.0;
        collectTimer = 0;
        collectRate = 0.5;
        maxLoad = 12;

        //relation to capitalist
        distToClosestCapitalist = 120.0f;
        id_ClosestCapitalist = 0;
        sensitivityCapitalist = 160.0f;
        capitalistNearby = false;
        businessDistance = 4.0f;
        tradeTimer = 0;
        unloadTimeCost = 30;

        //capitals
        resourceHoldings = 0.0;
        capitalHoldings = 30.0;
        poetryHoldings = 0.0;

        //human nature
        desireLevel = 0.5;
        separateForce = 1.5;

        //draw body
        scaleFactor = 0.3;
        mesh_Nv = addCone(body, capitalHoldings / 30.0, Vec3f(0,0,capitalHoldings / 30.0 * 3));
        for(int i=0; i<mesh_Nv; ++i){
			float f = float(i)/mesh_Nv;
			body.color(HSV(f*0.05,0.9,1));
		}
        addCube(resource, 4);
        resource.generateNormals();
        body.decompress();
        body.generateNormals();
    }

    void run(vector<Natural_Resource_Point>& nrps, vector<Miner>& others, vector<Capitalist>& capitalists){
        if (resourceHoldings < maxLoad){
            //resource mining
            senseResourcePoints(nrps);
            if (resourcePointFound == true){
                separateForce = 1.5;
                if (distToClosestNRP > sensitivityResource){
                    seekResourcePoint(nrps);
                } else if (distToClosestNRP < sensitivityResource && distToClosestResource >= 0) {
                    collectResource(nrps);
                    if (distToClosestResource < pickingRange){
                        collectTimer ++;
                        if (collectTimer % (int)floorf(60.0 / collectRate) == 0){
                            resourceHoldings += 1;
                            //cout << collectTimer << endl;
                        }
                        if (collectTimer >= (int)floorf(60.0 / collectRate) * 12 - 1){
                            collectTimer = 0;
                        }
                    }
                }
            } else {
                separateForce = 0.3;
                inherentDesire(desireLevel, FactoryRadius, NaturalRadius, desireChangeRate);
                facingToward(movingTarget);
            } 

            

        } else if (resourceHoldings >= maxLoad) {
            //find capitalist for a trade
            resourcePointFound = false;
            senseCapitalists(capitalists);
            if (capitalistNearby){
                if (distToClosestCapitalist > businessDistance){
                    seekCapitalist(capitalists);
                } else if (distToClosestCapitalist <= businessDistance && distToClosestCapitalist >= 0){
                    exchangeResource(capitalists);
                    tradeTimer ++;
                }
            } else {
                inherentDesire(desireLevel, FactoryRadius, NaturalRadius, desireChangeRate);
                facingToward(movingTarget);
            }
            if (tradeTimer == unloadTimeCost){
                resourceHoldings = 0;
                //capitalHoldings += 
                tradeTimer = 0;
            }

        }
        
        // cout << resourcePointFound << "found??" << endl;
        // cout << distToClosestNRP << " dist to closeset nrp"<< endl;
        // cout<< id_ClosestNRP << "  NRP" << endl;
        // cout << id_ClosestResource << " resource" << endl;

        //default behaviors
        Vec3f sep(separate(others));
        sep *= separateForce;
        applyForce(sep);     

        borderDetect();
        update();
    }
    void senseResourcePoints(vector<Natural_Resource_Point>& nrps){
        float min = 999;
        int min_id = 0;     
        for (int i = 0; i < nrps.size(); i++){
            if (!nrps[i].drained()){
                Vec3f dist_difference = pose.pos() - nrps[i].position;
                float dist = dist_difference.mag();
                if (dist < min){
                    min = dist;
                    min_id = i;
                    //update universal variable for other functions to use
                    distToClosestNRP = min;
                    id_ClosestNRP = min_id;
                }
            }
        }
        if (distToClosestNRP < sensitivityNRP){
            resourcePointFound = true;
        } else {
            resourcePointFound = false;
        }
    }
    void senseCapitalists(vector<Capitalist>& capitalists){
        int min_resources = 9999;
        int max_capitals = 0;
        int min_resource_id = 0;
        int max_rich_id = 0;
        for (int i = 0; i < capitalists.size(); i++){
            if (!capitalists[i].bankrupted()){

                //find the one needs resource
                if (capitalists[i].resourceHoldings < min_resources){
                    min_resources = capitalists[i].resourceHoldings;
                    min_resource_id = i;
                }
                //also find the one who is richest
                if (capitalists[i].capitalHoldings > max_capitals){
                    max_capitals = capitalists[i].capitalHoldings;
                    max_rich_id = i;
                }
            }
        }
        Vec3f dist_difference = pose.pos() - capitalists[min_resource_id].pose.pos();
        float dist_resource = dist_difference.mag();
        Vec3f dist_difference_2 = pose.pos() - capitalists[max_rich_id].pose.pos();
        float dist_rich = dist_difference_2.mag();
        
        if (dist_resource > dist_rich){
            distToClosestCapitalist = dist_resource;
            id_ClosestCapitalist = min_resource_id;
        } else if (dist_resource <= dist_rich){
            distToClosestCapitalist = dist_rich;
            id_ClosestCapitalist = max_rich_id;
        }

        if (distToClosestCapitalist < sensitivityCapitalist){
            capitalistNearby = true;
        } else {
            capitalistNearby = false;
        }
    }
    void seekResourcePoint(vector<Natural_Resource_Point>& nrps){
        if (!nrps[id_ClosestNRP].drained()){
            Vec3f skNRP(seek(nrps[id_ClosestNRP].position));
            skNRP *= searchResourceForce;
            applyForce(skNRP);
            facingToward(nrps[id_ClosestNRP].position);
        } 
    }
    void seekCapitalist(vector<Capitalist>& capitalists){
        if (!capitalists[id_ClosestCapitalist].bankrupted()){
            Vec3f skCP(seek(capitalists[id_ClosestCapitalist].pose.pos()));
            skCP *= 1.0;
            applyForce(skCP);
            Vec3f t = capitalists[id_ClosestCapitalist].pose.pos();
            facingToward(t);
        }
    }
    void exchangeResource(vector<Capitalist>& capitalists){
        Vec3f t = capitalists[id_ClosestCapitalist].pose.pos();
        Vec3f arCP(arrive(t));
        arCP *= 1.0;
        applyForce(arCP);
    }

    void collectResource(vector<Natural_Resource_Point>& nrps){
        float min = 9999;
        int min_id = 0;
        if (!nrps[id_ClosestNRP].drained()){
            for (int i = nrps[id_ClosestNRP].resources.size() - 1; i >= 0; i--){
                if (!nrps[id_ClosestNRP].resources[i].isPicked){
                    Vec3f dist_difference = pose.pos() - nrps[id_ClosestNRP].resources[i].position;
                    double dist = dist_difference.mag();
                    if (dist < min){
                        min = dist;
                        min_id = i;
                        id_ClosestResource = min_id;
                        distToClosestResource = dist;
                    }
                }
            }
            Vec3f collectNR(seek(nrps[id_ClosestNRP].resources[min_id].position));
            collectNR *= collectResourceForce;
            applyForce(collectNR);
            facingToward(nrps[id_ClosestNRP].resources[min_id].position);
        } 
    }
    void findPoems(){
        //30% probability
        if (rnd::prob(0.3)) {
            //find poems
        };
    }
    bool bankrupted(){
        return false;
    }
    Vec3f separate(vector<Miner>& others){
        Vec3f sum;
        int count = 0;
        for (Miner m : others){
            Vec3f difference = pose.pos() - m.pose.pos();
            float d = difference.mag();
            if ((d > 0) && (d<desiredseparation)){
                Vec3f diff = difference.normalize();
                sum += diff;
                count ++;
            }
        }
        if (count > 0){
            sum /= count;
            sum.mag(maxspeed);
            Vec3f steer = sum - velocity;
            if (steer.mag() > maxforce){
                steer.normalize(maxforce);
            }
            return steer;
        } else {
            return Vec3f(0,0,0);
        }
    }
    
    void draw(Graphics& g){
        g.pushMatrix();
        g.translate(pose.pos());
        g.rotate(pose.quat());
        g.scale(scaleFactor);
        if (resourceHoldings >= 12){ g.draw(resource);}
        g.draw(body);
        g.popMatrix();
    }

};

struct Worker : Agent {
    int mesh_Nv;
    Vec3f temp_pos;
    Vec3f workTarget;
    int desireChangeRate;
    float distToClosestFactory;
    int id_ClosestFactory;
    bool FactoryFound;
    float sensitivityFactory;
    float separateForce;
    float diligency;
    float mood;
    float workingDistance;
    float desireLevel;
    bool jobHunting;
    bool positionSecured;
    int patienceLimit;
    int patienceTimer;
    bool depression;
    Worker(){
        maxAcceleration = 1;
        mass = 1.0;
        maxspeed = 0.3;
        minspeed = 0.1;
        maxforce = 0.03;
        initialRadius = 5;
        target_senseRadius = 10.0;
        desiredseparation = 3.0f;
        acceleration = Vec3f(0,0,0);
        velocity = Vec3f(0,0,0);
        pose.pos() = r();
        temp_pos = pose.pos();
        pose.pos() = pose.pos() * (FactoryRadius - MetroRadius) + temp_pos.normalize(MetroRadius);
        bioClock = 0;
        movingTarget = r();
        workTarget = r();

        //human nature
        desireLevel = 0.5;
        desireChangeRate = r_int(60, 60); //60 ~ 120
        diligency = rnd::uniform(0.7, 1.4); //0.7 ~ 1.4
        mood = r_int(30, 90);
        patienceLimit = r_int(30, 120);
        patienceTimer = 0;

        //relation to factory
        distToClosestFactory = 200;
        id_ClosestFactory = 0;
        sensitivityFactory = 90;
        workingDistance = 8;
        jobHunting = true;
        positionSecured = false;
        FactoryFound = false;
        depression = false;

        //capitals
        capitalHoldings = 30.0;
        poetryHoldings = 0.0;
        

        //draw body
        scaleFactor = 0.3;
        mesh_Nv = addCone(body, capitalHoldings / 30.0, Vec3f(0,0,capitalHoldings / 30.0 * 3));
        for(int i=0; i<mesh_Nv; ++i){
			float f = float(i)/mesh_Nv;
			body.color(HSV(f*0.65,0.9,1));
		}
        body.decompress();
        body.generateNormals();
    }
    void run(vector<Factory>& fs, vector<Worker>& others, vector<Capitalist>& capitalist){
        if (jobHunting){
            senseFactory(fs);
        }

        if (depression){
            jobHunting = true;
            if (capitalHoldings <= 30){
                seekCapitalist(capitalist);
                separateForce = 1.5;
            } else {
                inherentDesire(desireLevel, MetroRadius, FactoryRadius, desireChangeRate);
                facingToward(movingTarget);
            }
        } else {
            if (FactoryFound){
                if (distToClosestFactory > workingDistance){
                    seekFactory(fs);
                    separateForce = 0.3;               
                } else if (distToClosestFactory <= workingDistance && distToClosestFactory >= 0){
                    work(diligency, mood, fs[id_ClosestFactory].meshOuterRadius, fs);  
                    if (fs[id_ClosestFactory].workersWorkingNum <= fs[id_ClosestFactory].workersNeededNum){
                        jobHunting = false;
                        //earn salary here!!
                    } else {
                        //think about jobhunting, while waiting for other people to opt out first
                        patienceTimer += 1;
                        if (patienceTimer == patienceLimit){
                            jobHunting = true;
                            patienceTimer = 0;
                        }
                        
                    }
                }
            } else {
                inherentDesire(desireLevel, MetroRadius, FactoryRadius, desireChangeRate);
                facingToward(movingTarget);
            }
        }
        //default behaviors
        Vec3f sep(separate(others));
        sep *= separateForce;
        applyForce(sep);     

        borderDetect();
        update();
    }
    bool bankrupted(){
        return false;
    }
    void senseFactory(vector<Factory>& fs){
        float min = 999;
        int min_id = 0;
        int openingCount = 0;     
        for (int i = 0; i < fs.size(); i++){
            if (fs[i].operating() && fs[i].hiring){
                openingCount += 1;
                Vec3f dist_difference = pose.pos() - fs[i].position;
                float dist = dist_difference.mag();
                if (dist < min){
                    min = dist;
                    min_id = i;
                    distToClosestFactory = min;
                    id_ClosestFactory = min_id;
                }
            }
        }
        //cout << openingCount << endl;
        if (openingCount == 0){
            depression = true;
        } else {
            depression = false;
        }
        if (distToClosestFactory < sensitivityFactory){
            FactoryFound = true;
        } else {
            FactoryFound = false;
        }
    }
    void seekFactory(vector<Factory>& fs){
        if (fs[id_ClosestFactory].operating()){
            Vec3f skFS(seek(fs[id_ClosestFactory].position));
            skFS *= 1.0;
            applyForce(skFS);
            Vec3f t = fs[id_ClosestFactory].position;
            facingToward(t);
        }
    }
    void seekCapitalist(vector<Capitalist>& capitalist){
        if (!capitalist[id_ClosestFactory].bankrupted()){
            Vec3f skCP(seek(capitalist[id_ClosestFactory].pose.pos()));
            skCP *= 1.0;
            applyForce(skCP);
            Vec3f t = capitalist[id_ClosestFactory].pose.pos();
            facingToward(t);
        } else {
            inherentDesire(desireLevel, MetroRadius, FactoryRadius, desireChangeRate);
            facingToward(movingTarget);
        }
    }
    void work(float diligency, int mood, float radius, vector<Factory>& fs){
        if (bioClock == 0){
            workTarget = r() * radius + fs[id_ClosestFactory].position;
        }
        if (bioClock % mood == 0) {
            workTarget = r() * radius + fs[id_ClosestFactory].position;
        }
        if (bioClock >= mood * 12 - 1){
            bioClock = 0;
            mood = r_int(30, 90);
        }
        bioClock ++;

        Vec3f workAround(seek(workTarget));
        workAround *= diligency;
        applyForce(workAround);
        facingToward(workTarget);
    }
    void findPoems(){
        //30% probability
        if (rnd::prob(0.3)) {
            //find poems
        };
    }
    Vec3f separate(vector<Worker>& others){
        Vec3f sum;
        int count = 0;
        for (Worker w : others){
            Vec3f difference = pose.pos() - w.pose.pos();
            float d = difference.mag();
            if ((d > 0) && (d < desiredseparation)){
                Vec3f diff = difference.normalize();
                sum += diff;
                count ++;
            }
        }
        if (count > 0){
            sum /= count;
            sum.mag(maxspeed);
            Vec3f steer = sum - velocity;
            if (steer.mag() > maxforce){
                steer.normalize(maxforce);
            }
            return steer;
        } else {
            return Vec3f(0,0,0);
        }
    }
    void draw(Graphics& g){
        g.pushMatrix();
        g.translate(pose.pos());
        g.rotate(pose.quat());
        g.scale(scaleFactor);
        g.draw(body);
        g.popMatrix();
    }


};



#include "locations.hpp"

#endif