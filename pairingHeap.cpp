#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include "pairingHeap.h"

using namespace std;



int main() {

    //testing params
    int n = 50;
    int e = 5;
    int a = 10;
    int lower_bound = 0;
    int upper_bound = 1000;

    //used in recursive print function, initially empty
    vector<Node*> parents;

    //init random number gen for testing
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    uniform_int_distribution<> distrib(lower_bound, upper_bound);

    PairingHeap pHeap;

    for(int i = 0; i < n; i++) {
        pHeap.insert(distrib(gen));
    }
    for(int i = 0; i < e; i++) {
        Node* minNode = pHeap.extractMin();
        cout << "Extracted min: " << minNode->key << "\n";
        pHeap.printHeap(parents);
    }
    for(int i = 0; i < a; i++) {
        cout << "Inserted node: " << pHeap.insert(distrib(gen))->key << "\n";
        pHeap.printHeap(parents);
    }

    
    //pHeap.printHeap(parents);
}