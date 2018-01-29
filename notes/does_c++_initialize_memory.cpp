#include <iostream>
const int N = 100000000;
int main() {
    char* memory = new char[N];
    for (int i = 0; i < N; i ++) {
        if (memory[i] !=0){
            printf("no\n");
            return 0;
        }
        printf("yes\n"); //c++ initialize memory
    }
}