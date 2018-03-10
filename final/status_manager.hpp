#ifndef INCLUDE_STATUS_MANAGER_HPP
#define INCLUDE_STATUS_MANAGER_HPP

#include "allocore/io/al_App.hpp"
#include "agent_managers.hpp"
#include "location_managers.hpp"

using namespace al;
using namespace std;


struct MarketManager{
    int numCapitalists;
    int numWorkers;
    int numMiners;
    int liveCapitalists;
    int liveWorkers;
    int liveMiners;
    int jobHuntingWorkers;
    int poorCapitalists;
    int poorWorkers;
    int poorMiners;
    int richWorkers;
    int richMiners;
    float averageCollectRate;
    float averageMaxLoad;
    float averageMinerSpeed;
    float resourceUnitPrice;
    float laborUnitPrice;

    MarketManager(){
        numCapitalists = 1;
        numWorkers = 1;
        numMiners = 1;
        liveCapitalists = 1;
        liveWorkers = 1;
        liveMiners = 1;
        jobHuntingWorkers = 1;

        //miners stats
        averageCollectRate = 0.5;
        averageMaxLoad = 12;
        averageMinerSpeed = 0.3;

        //market
        resourceUnitPrice = 150;
        laborUnitPrice = 250;
    }
    void statsInit(Capitalist_Entity& capitalists, Worker_Union& workers, Miner_Group& miners){
        numCapitalists = capitalists.cs.size();
        liveCapitalists = numCapitalists;
        numWorkers = workers.workers.size();
        liveWorkers = numWorkers;
        jobHuntingWorkers = workers.workers.size();
        numMiners = miners.ms.size();
        liveMiners = numMiners;

        //calculate Miner stats
        float sum1 = 0;
        float sum2 = 0;
        float sum3 = 0;
        for (int i = 0; i < miners.ms.size(); i++){
            sum1 += miners.ms[i].collectRate;
            sum2 += miners.ms[i].maxLoad;
            sum3 += miners.ms[i].maxspeed;
        }
        averageCollectRate = sum1 / miners.ms.size();
        averageMaxLoad = sum2 / miners.ms.size();
        averageMinerSpeed = sum3 / miners.ms.size();
        cout << averageCollectRate << "avg collectrate" << endl;
        cout << averageMaxLoad << "avg maxload "<< endl;
        cout << averageMinerSpeed << "avg miner max speed " << endl;

        //initialize price
        resourceUnitPrice = (1 / averageCollectRate * 60 + NaturalRadius / averageMaxLoad / averageMinerSpeed * 2) * (numMiners * 2 / (1 + liveMiners));
        //time is money, frames needed for each resource + travel distance / velocity divided by each resource, plus some extra time
        //the more miners, the cheaper resource, and vice versa
        //resource per second * frameRate = minimum Money consumption rate each frame
        laborUnitPrice = resourceUnitPrice + resourceUnitPrice * ( (numWorkers - jobHuntingWorkers) / liveWorkers);
    }
    void populationMonitor(Capitalist_Entity& capitalists, Worker_Union& workers, Miner_Group& miners){
        liveCapitalists = 0;
        liveWorkers = 0;
        liveMiners = 0;
        jobHuntingWorkers = 0;
        for (int i = 0; i < capitalists.cs.size(); i ++){
            if (!capitalists.cs[i].bankrupted()){
                liveCapitalists += 1;
                if (capitalists.cs[i].capitalHoldings < 20){

                }
            }
        }
        for (int i = 0; i < workers.workers.size(); i ++){
            if (!workers.workers[i].bankrupted()){
                liveWorkers += 1;
                if (workers.workers[i].jobHunting == true){
                    jobHuntingWorkers += 1;
                }
            }
        }
        for (int i = 0; i < miners.ms.size(); i++){
            if (!miners.ms[i].bankrupted()){
                liveMiners += 1;
            }
        }
        //cout << liveCapitalists << " cp " << liveWorkers << " ws " << liveMiners << " ms " << endl;
    }
    void updatePrice(Capitalist_Entity& capitalists, Worker_Union& workers, Miner_Group& miners){
        resourceUnitPrice = (1 / averageCollectRate * 60 + NaturalRadius / averageMaxLoad / averageMinerSpeed * 2) * (numMiners * 2 / (1 + liveMiners));
        
    }
    void monitorResourceStatus(vector<Natural_Resource_Point> nrps){

    }
    
};


#endif