#include <iostream>
#include <future>

void called_from_async(){
    std::cout << "async all" <<std::endl;
    return 42;
}
int main(){
    std::future<void> result(std::async(called_from_async()));
    std::cout << "message from main." << std::endl;
    std::cout << "result was " << result.get() << std::endl;
    return 0;
}