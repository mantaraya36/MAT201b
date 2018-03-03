#include <iostream>
#include <thread>

void dothings(){
    printf("done\n");
}

int main(){
    std::thread t(dothings);
    printf("got here!\n");
    t.join();
    //you can't be sure which happens first
}