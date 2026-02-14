#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <climits>
#include "pairingHeap(2).h"
#include "Fibonacciheap.cpp"

using namespace std;

/*use heap to hold min edge value:
    create graph object with edge values
    extract min (first node)
    update nodes min-node had edges with with edge values (key)
        check if connected node extracted
            if not
                update key value as edge from min-node to node
*/

//A graph node
class GraphNode {
public:
    int dest; //the vertex number / id of the node
    int weight; //the key value (determined by edge values)
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

        //adds node to adj list at end value
        newNode = new GraphNode(start, value);
        newNode->next = array[end].first;
        array[end].first = newNode;
    }
};

//Prims with pairing heap
void PrimsPairing(Graph* graph){
    int vert = graph->vertices;//number of vertices
    vector<int> values; //holds weight values for comparison
    values.resize(vert);
    vector<int> final; //will hold final tree//u->final[v]
    final.resize(vert);
    vector<Node*> nodeArray; //Pairing heap node array. store node from insertKey opp.
    nodeArray.resize(vert);
    PairingHeap* pairHeap = new PairingHeap;//Pairing heap for graph nodes
    chrono::microseconds minTime;
    chrono::microseconds decreaseTime;
    minTime = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - chrono::system_clock::now());
    decreaseTime = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - chrono::system_clock::now());

    //fill arrays and heap
    //except first vertex
    for (int i = 1; i < vert; i++){
        values[i] = INT_MAX; //should be int_max
        final[i] = -1;
        Node* newNode = pairHeap->insert(INT_MAX, i);
        nodeArray[i] = newNode;
    }

    //Set first vertex to be 0
    values[0] = 0;        
    Node* newNode = pairHeap->insert(0, 0);
    nodeArray[0] = newNode;

    //TODO ASK FOR EMPTY TREE OPP???
    while(!pairHeap->isEmpty(pairHeap->root)){
        auto timerStart = chrono::system_clock::now();
        newNode = pairHeap->extractMin(); //Pulls min from heap
        auto timerEnd = chrono::system_clock::now();
        auto tempTime = chrono::duration_cast<chrono::microseconds>(timerEnd-timerStart);
        minTime += tempTime;
        GraphNode* search = graph->array[newNode->vertex].first; //sets search as the first node in the minNode's adj list

        while(search != nullptr){
            int destination = search->dest;
            //If note extracted yet(if pointer in array = nullptr), 
            if(nodeArray[destination] != nullptr && search->weight < values[destination]){
                values[destination] = search->weight; //set value to new value
                final[destination] = newNode->vertex;  //set that destination will connect to vertex ( [i] connects to-> v)
                timerStart = chrono::system_clock::now();
                pairHeap->decreaseKey(nodeArray[destination], search->weight); //update key of pairing heap node
                timerEnd = chrono::system_clock::now();
                tempTime = chrono::duration_cast<chrono::microseconds>(timerEnd-timerStart);
                decreaseTime += tempTime;   
            }
            if(search->next != nullptr){
            }
            search = search->next;
        }
    }

    //Outputs completed array
    for(int i = 0; i < graph->vertices; i++){
        cout << i << " - " << final[i] << endl;
    }

    cout << "min extract time: " << minTime.count() << endl;
    cout << "decrease key time: " << decreaseTime.count() << endl;
}

//Prims with Fib heap
void PrimsFib(Graph* graph){
    int vert = graph->vertices;
    vector<int> final;//will hold final connections
    final.resize(vert);
    vector<int> values;//holds comparision values
    values.resize(vert);
    vector<FibNode*> nodeArray; //Pairing heap node array. store node from insertKey opp.
    nodeArray.resize(vert);
    FibonacciHeap* fibHeap = new FibonacciHeap;
    FibNode* tempNode;
    chrono::microseconds minTime;
    chrono::microseconds decreaseTime;
    minTime = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - chrono::system_clock::now());
    decreaseTime = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - chrono::system_clock::now());

    for(int i = 1; i < vert; i++){
        values.at(i) = INT_MAX;
        final.at(i) = -1;
        tempNode = fibHeap->insert(INT_MAX);
        tempNode->num = i;
        nodeArray.at(i) = tempNode;
    }

    //Set first vertex to be 0
    values[0] = 0;        
    tempNode = fibHeap->insert(0);
    tempNode->num = 0;
    nodeArray[0] = tempNode;

    while(fibHeap->getMin() != nullptr){
        auto timerStart = chrono::system_clock::now();
        FibNode* minNode = fibHeap->extractMin();
        auto timerEnd = chrono::system_clock::now();
        auto tempTime = chrono::duration_cast<chrono::microseconds>(timerEnd-timerStart);
        minTime += tempTime;
        int u = minNode->num;//min vertex number
        GraphNode* search = graph->array.at(u).first;

        while(search != nullptr){
            int v = search->dest;

            if(nodeArray.at(v) != nullptr && search->weight < values.at(v)){//if in the array
                values.at(v) = search->weight;//set comparision values
                final.at(v) = u;//set better edge
                timerStart = chrono::system_clock::now();
                fibHeap->decreaseKey(nodeArray.at(v), search->weight);//set node in heap to new value
                timerEnd = chrono::system_clock::now();
                tempTime = chrono::duration_cast<chrono::microseconds>(timerEnd-timerStart);
                decreaseTime += tempTime;
            }
            search = search->next;
        }
    }

    for(int i = 0; i < vert; i++){
        cout << i << " - " << final.at(i) << endl;
    }

    cout << "min extract time: " << minTime.count() << endl;
    cout << "decrease key time: " << decreaseTime.count() << endl;
}


void Binary(Graph* graph){
    int vert = graph->vertices;
    vector<int> final;//will hold final connections
    final.resize(vert);
    vector<int> values;//holds comparision values
    values.resize(vert);
    vector<FibNode*> nodeArray; //Pairing heap node array. store node from insertKey opp.
    nodeArray.resize(vert);
    FibonacciHeap* fibHeap = new FibonacciHeap;
    FibNode* tempNode;

    for(int i = 1; i < vert; i++){
        values.at(i) = INT_MAX;
        final.at(i) = -1;
        tempNode = fibHeap->insert(INT_MAX);
        tempNode->num = i;
        nodeArray.at(i) = tempNode;
    }

        //Set first vertex to be 0
    values[0] = 0;        
    tempNode = fibHeap->insert(0);
    tempNode->num = 0;
    nodeArray[0] = tempNode;

    while(fibHeap->getMin() != nullptr){
        FibNode* minNode = fibHeap->extractMin();
        int u = minNode->num;//min vertex number
        GraphNode* search = graph->array.at(u).first;

        while(search != nullptr){
            int v = search->dest;

            if(nodeArray.at(v) != nullptr && search->weight < values.at(v)){//if in the array
                values.at(v) = search->weight;//set comparision values
                final.at(v) = u;//set better edge
                fibHeap->decreaseKey(nodeArray.at(v), search->weight);//set node in heap to new value
            }
            search = search->next;
        }
    }

    for(int i = 0; i < vert; i++){
        cout << i << " - " << final.at(i) << endl;
    }


}

int main() {
    
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

    PrimsPairing(gridGraph);

    auto endOverall = chrono::system_clock::now();

    auto overallTime = chrono::duration_cast<chrono::microseconds>(endOverall-startOverall);

    cout << "Overall Time: " << overallTime.count() << " microseconds" << endl;

    return 0;
}