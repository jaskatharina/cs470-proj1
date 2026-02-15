#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <chrono>
#include <climits>
#include "pairingHeap.h"
#include "Fibonacciheap.cpp"
#include "metrics.h"
#include <bits/stdc++.h>



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

void DijkstraPairing(Graph* graph, int source, Metrics* metrics = nullptr){
    int vert = graph->vertices;
    vector<int> values; //hold values for comparison
    values.resize(vert);
    vector<Node*> nodeArray;//array of nodes
    nodeArray.resize(vert);
    PairingHeap* pairHeap = new PairingHeap;

        //fill arrays and heap
    for (int i = 0; i < vert; i++){
        values[i] = INT_MAX; //should be int_max
        nodeArray.at(i) = pairHeap->insert(INT_MAX, i);
    }

    
    pairHeap->decreaseKey(nodeArray.at(source), 0);
    values.at(source) = 0;

    while(!(pairHeap->isEmpty())){
        if(metrics) metrics->startExtractMin();
        Node* min = pairHeap->extractMin();
        if(metrics) metrics->stopExtractMin();
        int v = min->vertex;
        GraphNode* search = graph->array[min->vertex].first;

        while(search != nullptr){
            int destination = search->dest;

            if(nodeArray.at(destination) != nullptr && values.at(v) != INT_MAX && (search->weight + values.at(v)) < values.at(destination) ){
                values.at(destination) = values.at(v) + search->weight;
                if(metrics) metrics->startDecreaseKey();
                pairHeap->decreaseKey(nodeArray.at(destination), values.at(destination));
                if(metrics) metrics->stopDecreaseKey();
            }
            search = search->next;
        }
    }

    // for(int i = 0; i < vert; i++){
    //     cout << "vertex: " << i << " ; Distance: " << values[i] << endl;
    // }
}

void DijkstraFib(Graph* graph, int source, Metrics* metrics = nullptr){
    int vert = graph->vertices;
    vector<int> values;
    values.resize(vert);
    vector<FibNode*> nodeArray;
    nodeArray.resize(vert);
    FibonacciHeap* fibHeap = new FibonacciHeap;

    for(int i = 0; i < vert; i++){
        values.at(i) = INT_MAX;
        nodeArray.at(i) = fibHeap->insert(INT_MAX, i);
    }

    //set values for source node to be zero
    fibHeap->decreaseKey(nodeArray.at(source), 0);
    values.at(source) = 0;

    while (!(fibHeap->isEmpty())){
        if(metrics) metrics->startExtractMin();
        FibNode* minNode = fibHeap->extractMin();
        if(metrics) metrics->stopExtractMin();
        int u = minNode->num;
        GraphNode* search = graph->array.at(u).first;

        //travels though connected list
        while(search != nullptr){
            int v = search->dest;

            //if not extracted and if value through minNode is less than current value, update current value
            if(nodeArray.at(v) != nullptr && values.at(u) != INT_MAX && (search->weight + values.at(u)) < values.at(v)){
                values.at(v) = values.at(u) + search->weight;
                if(metrics) metrics->startDecreaseKey();
                fibHeap->decreaseKey(nodeArray.at(v), values.at(v));
                if(metrics) metrics->stopDecreaseKey();
            }
            
            search = search->next;
        }
    }

    // for(int i = 0; i < vert; i++){
    //     cout << i << " distance from source: " << values.at(i) << endl;
    // }
}
int main() {

    const int RUNS = 10;

    // Lambda to compute mean and SD
    auto computeMeanSD = [](auto &v, double &mean, double &sd) {
        int n = v.size();
        mean = 0;
        for (auto x : v) mean += x;
        mean /= n;

        sd = 0;
        for (auto x : v) sd += (x - mean) * (x - mean);
        sd = sqrt(sd / n);
    };

    // Open CSV file
    ofstream csvFile("dijkstra_results.csv");
    csvFile << "n,total_mean,total_sd,decrease_mean,decrease_sd,extract_mean,extract_sd,ops_mean,ops_sd,heap_type,graph_type\n";

    for (int v = 10; v <= 1000; v *= 10) {

        for (auto graph_type : {"Sparse", "Dense"}) {

            cout << "====================================\n";
            cout << graph_type << " graph, size n = " << v << "\n";

            // Vectors to store per-run metrics 
            vector<double> fib_total(RUNS), fib_dec(RUNS), fib_ex(RUNS);
            vector<int> fib_ops(RUNS);

            vector<double> pair_total(RUNS), pair_dec(RUNS), pair_ex(RUNS);
            vector<int> pair_ops(RUNS);

            for (int r = 0; r < RUNS; r++) {

                Graph* newGraph = new Graph(v);

                unsigned seed = 42 + r;
                mt19937 gen(seed);
                uniform_int_distribution<> vertexDistrib(0, v - 1);
                uniform_int_distribution<> weightDistrib(0, 100);

                // Determine number of edges
                int numEdges;
                if (string(graph_type) == "Sparse") {
                    numEdges = v; // ~1 edge per vertex
                } else {
                    double density = 0.2; // 20% of all possible edges
                    numEdges = static_cast<int>(v * (v - 1) / 2 * density);
                }

                // Use a set to avoid duplicate edges
                set<pair<int,int>> addedEdges;

                // Ensure connectivity first (chain)
                for (int i = 0; i < v-1; i++) {
                    newGraph->addEdge(i, i+1, weightDistrib(gen));
                    addedEdges.insert({i, i+1});
                }

                // Add remaining edges randomly
                while ((int)addedEdges.size() < numEdges) {
                    int u = vertexDistrib(gen);
                    int w = vertexDistrib(gen);
                    if (u == w) continue;
                    auto e = minmax(u, w);
                    if (addedEdges.count(e)) continue;
                    newGraph->addEdge(e.first, e.second, weightDistrib(gen));
                    addedEdges.insert(e);
                }

                Metrics metrics;

                // Fibonacci Heap
                metrics.startRun();
                DijkstraFib(newGraph, 0, &metrics);
                metrics.stopRun();

                fib_total[r] = metrics.totalRunTime();
                fib_dec[r]   = metrics.decreaseKeyTime();
                fib_ex[r]    = metrics.extractMinTime();
                fib_ops[r]   = metrics.heapOps();
                metrics.clear();

                // Pairing Heap
                metrics.startRun();
                DijkstraPairing(newGraph, 0, &metrics);
                metrics.stopRun();

                pair_total[r] = metrics.totalRunTime();
                pair_dec[r]   = metrics.decreaseKeyTime();
                pair_ex[r]    = metrics.extractMinTime();
                pair_ops[r]   = metrics.heapOps();

                delete newGraph;
            }

            // Fibonacci Heap metrics
            double mean, sd;
            computeMeanSD(fib_total, mean, sd); double total_mean = mean, total_sd = sd;
            computeMeanSD(fib_dec, mean, sd);   double dec_mean = mean, dec_sd = sd;
            computeMeanSD(fib_ex, mean, sd);    double ex_mean = mean, ex_sd = sd;
            computeMeanSD(fib_ops, mean, sd);   double ops_mean = mean, ops_sd = sd;

            cout << "Dijkstra (Fibonacci Heap) — Total runtime: " << total_mean << " s ± " << total_sd << " s\n";
            csvFile << v << "," << total_mean << "," << total_sd << ","
                    << dec_mean << "," << dec_sd << ","
                    << ex_mean << "," << ex_sd << ","
                    << ops_mean << "," << ops_sd << ",Fibonacci," << graph_type << "\n";

            // Pairing Heap metrics
            computeMeanSD(pair_total, mean, sd); total_mean = mean; total_sd = sd;
            computeMeanSD(pair_dec, mean, sd);   dec_mean = mean; dec_sd = sd;
            computeMeanSD(pair_ex, mean, sd);    ex_mean = mean; ex_sd = sd;
            computeMeanSD(pair_ops, mean, sd);   ops_mean = mean; ops_sd = sd;

            cout << "Dijkstra (Pairing Heap) — Total runtime: " << total_mean << " s ± " << total_sd << " s\n";
            csvFile << v << "," << total_mean << "," << total_sd << ","
                    << dec_mean << "," << dec_sd << ","
                    << ex_mean << "," << ex_sd << ","
                    << ops_mean << "," << ops_sd << ",Pairing," << graph_type << "\n";
        }
    }

    csvFile.close();
    cout << "\nCSV results written to dijkstra_results.csv\n";

    return 0;
}
