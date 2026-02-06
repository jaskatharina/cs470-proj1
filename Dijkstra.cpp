#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <climits>
#include "pairingHeap(1).h"
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

        //fill arrays and heap
    for (int i = 0; i < vert; i++){
        values[i] = INT_MAX; //should be int_max
        Node* newNode = pairHeap->insert(INT_MAX, i);
        nodeArray[i] = newNode;
    }

    values[source] = 0;
    pairHeap->decreaseKey(nodeArray[source], 0);

    while(!pairHeap->isEmpty(pairHeap->root)){
        Node* tempNode = pairHeap->extractMin();
        int v = tempNode->vertex;

        GraphNode* search = graph->array[tempNode->vertex].first;

        while(search != nullptr){
            int destination = search->dest;

            if(nodeArray[destination] != nullptr && values[v] != INT_MAX && (search->weight + values[v]) < values[destination] ){
                values[destination] = values[v] + search->weight;
                pairHeap->decreaseKey(nodeArray[destination], values[destination]);
            }

            search = search->next;
        }
    }

    for(int i = 0; i < values.size(); i++){
        cout << "vertex: " << i << " ; Distance: " << values[i] << endl;
    }
}

void DijkstraFib(Graph* graph, int source){
    int vert = graph->vertices;
    vector<int> values;
    values.resize(vert);
    vector<FibNode*> nodeArray;
    nodeArray.resize(vert);
    FibonacciHeap* fibHeap = new FibonacciHeap;
    FibNode* tempNode;

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
        FibNode* minNode = fibHeap->extractMin();
        int u = minNode->num;
        GraphNode* search = graph->array.at(u).first;

        //travels though connected list
        while(search != nullptr){
            int v = search->dest;

            //if not extracted and if value through minNode is less than current value, update current value
            if(nodeArray.at(v) != nullptr && values[u] != INT_MAX && (search->weight + values.at(u)) < values.at(v)){
                values.at(v) = values.at(u) + search->weight;
                fibHeap->decreaseKey(nodeArray.at(v), values.at(v));
            }
            
            search = search->next;
        }
    }

    for(int i = 0; i < vert; i++){
        cout << i << " distance from source: " << values.at(i) << endl;
    }
}

int main(){

    int V = 4;
    Graph* newGraph = new Graph(V);

    //Pairing heap crashing for some reason????
    newGraph->addEdge(0, 1, 1);
    newGraph->addEdge(0, 2, 3);
    newGraph->addEdge(1, 2, 1);
    newGraph->addEdge(1, 3, 2);
    newGraph->addEdge(2, 3, 5);

    DijkstraFib(newGraph, 0);

    return 0;
}