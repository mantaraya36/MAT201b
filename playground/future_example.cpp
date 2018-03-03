#include <iostream>
#include <future>

int twice(int m){
    usleep(rand() / RAND_MAX * 0.1 + 1000);
    return 2 * m;
}
int main(){
    std::vector<future> futures;
    for (int i = 0; i < futures.size(); i ++){
        futures.push_back(std::async(twice(i)));
    }
}