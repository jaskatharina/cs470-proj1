#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <climits>
#include "pairingHeap(2).h"
#include "Fibonacciheap.cpp"


using namespace std;

//A graph node
class GraphNode {
public:
    int dest;
    int weight;
    GraphNode* next;
    GraphNode(int newDest, int newWeight){
        dest = newDest;
        weight = newWeight;
        next = NULL;
    }
};

//Linked list of graph nodes
class GraphNodeList {
public:
    GraphNode* first; //pointer to first node of list    
};

//Vector of graph-node lists
class Graph {
public:
    int vertices;
    vector<GraphNodeList> array;
    Graph(int v){
        vertices = v;
        array.resize(v);
        for(int i = 0; i < v; i++){//set all adjLists as null to start
            array.at(i).first = NULL;
        }
    }

    //use this to add a node to the graph
    void addEdge(int start, int end, int value){
        //adds node to adj list at 'start' value
        GraphNode* newNode = new GraphNode(end, value);
        newNode->next = array[start].first;
        array[start].first = newNode;

        newNode = new GraphNode(start, value);
        newNode->next = array[end].first;
        array[end].first = newNode;
    }
};

void DijkstraPairing(Graph* graph, int source){
    int vert = graph->vertices;
    vector<int> values; //hold values for comparison
    values.resize(vert);
    vector<Node*> nodeArray;//array of nodes
    nodeArray.resize(vert);
    PairingHeap* pairHeap = new PairingHeap;
    chrono::microseconds minTime;
    chrono::microseconds decreaseTime;
    minTime = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - chrono::system_clock::now());
    decreaseTime = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - chrono::system_clock::now());

        //fill arrays and heap
    for (int i = 0; i < vert; i++){
        values[i] = INT_MAX; //should be int_max
        Node* newNode = pairHeap->insert(INT_MAX, i);
        nodeArray[i] = newNode;
    }

    values[source] = 0;
    pairHeap->decreaseKey(nodeArray[source], 0);

    while(!pairHeap->isEmpty(pairHeap->root)){
        auto timerStart = chrono::system_clock::now();
        Node* tempNode = pairHeap->extractMin();
        int v = tempNode->vertex;
        auto timerEnd = chrono::system_clock::now();
        auto tempTime = chrono::duration_cast<chrono::microseconds>(timerEnd-timerStart);
        minTime += tempTime;
        GraphNode* search = graph->array[tempNode->vertex].first;

        while(search != nullptr){
            int destination = search->dest;
            if(nodeArray[destination] != nullptr && values[v] != INT_MAX && (search->weight + values[v]) < values[destination] ){
                values[destination] = values[v] + search->weight;
                timerStart = chrono::system_clock::now();
                pairHeap->decreaseKey(nodeArray[destination], values[destination]);
                timerEnd = chrono::system_clock::now();
                tempTime = chrono::duration_cast<chrono::microseconds>(timerEnd-timerStart);
                decreaseTime += tempTime;
            }

            search = search->next;
        }
    }

    for(int i = 0; i < values.size(); i++){
        cout << "vertex: " << i << " ; Distance: " << values[i] << endl;
    }
    cout << "min extract time: " << minTime.count() << endl;
    cout << "decrease key time: " << decreaseTime.count() << endl;
}

void DijkstraFib(Graph* graph, int source){
    int vert = graph->vertices;
    vector<int> values;
    values.resize(vert);
    vector<FibNode*> nodeArray;
    nodeArray.resize(vert);
    FibonacciHeap* fibHeap = new FibonacciHeap;
    FibNode* tempNode;
    chrono::microseconds minTime;
    chrono::microseconds decreaseTime;
    minTime = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - chrono::system_clock::now());
    decreaseTime = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - chrono::system_clock::now());

    for(int i = 0; i < vert; i++){
        values.at(i) = INT_MAX;
        tempNode = fibHeap->insert(INT_MAX);
        tempNode->num = i;
        nodeArray.at(i) = tempNode;
    }

    //set values for source node to be zero
    tempNode = nodeArray.at(source);
    fibHeap->decreaseKey(tempNode, 0);
    values.at(source) = 0;

    while (fibHeap->getMin() != nullptr){
        auto timerStart = chrono::system_clock::now();
        FibNode* minNode = fibHeap->extractMin();
        auto timerEnd = chrono::system_clock::now();
        auto tempTime = chrono::duration_cast<chrono::microseconds>(timerEnd-timerStart);
        minTime += tempTime;
        int u = minNode->num;
        GraphNode* search = graph->array.at(u).first;

        //travels though connected list
        while(search != nullptr){
            int v = search->dest;

            //if not extracted and if value through minNode is less than current value, update current value
            if(nodeArray.at(v) != nullptr && values[u] != INT_MAX && (search->weight + values.at(u)) < values.at(v)){
                values.at(v) = values.at(u) + search->weight;
                auto timerStart = chrono::system_clock::now();
                fibHeap->decreaseKey(nodeArray.at(v), values.at(v));
                timerEnd = chrono::system_clock::now();
                tempTime = chrono::duration_cast<chrono::microseconds>(timerEnd-timerStart);
                decreaseTime += tempTime;
            }
            
            search = search->next;
        }
    }

    for(int i = 0; i < vert; i++){
        cout << i << " distance from source: " << values.at(i) << endl;
    }
    cout << "min extract time: " << minTime.count() << endl;
    cout << "decrease key time: " << decreaseTime.count() << endl;
}

int main(){

auto startOverall = chrono::system_clock::now();
    int v = 9;

    //example: Grid Graph
    Graph* gridGraph = new Graph(v);
    gridGraph->addEdge(0, 1, 1);
    gridGraph->addEdge(0, 3, 3);
    gridGraph->addEdge(1, 2, 1);
    gridGraph->addEdge(1, 4, 4);
    gridGraph->addEdge(2, 5, 1);
    gridGraph->addEdge(3, 4, 10);
    gridGraph->addEdge(3, 6, 1);
    gridGraph->addEdge(4, 5, 5);
    gridGraph->addEdge(5, 8, 1);
    gridGraph->addEdge(6, 7, 1);
    gridGraph->addEdge(7, 8, 10);

    //worst case graph? not quite sure
    Graph* worstGraph = new Graph(v);
    worstGraph->addEdge(0, 1, 1);
    worstGraph->addEdge(0, 8, 8);
    worstGraph->addEdge(1, 2, 1);
    worstGraph->addEdge(2, 3, 1);
    worstGraph->addEdge(3, 4, 1);
    worstGraph->addEdge(4, 5, 1);
    worstGraph->addEdge(5, 6, 1);
    worstGraph->addEdge(6, 7, 1);
    worstGraph->addEdge(7, 8, 1);

    DijkstraPairing(worstGraph, 0);

    auto endOverall = chrono::system_clock::now();

    auto overallTime = chrono::duration_cast<chrono::microseconds>(endOverall-startOverall);

    cout << "Overall Time: " << overallTime.count() << " microseconds" << endl;

    return 0;
}