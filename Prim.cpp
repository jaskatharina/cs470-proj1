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
    void addEdge(int start, int end, int value) {
        //if start and end node end up being the same, do nothing
        if(start == end) {
            return;
        }
        //adds node to adj list at 'start' value
        GraphNode* newNode = new GraphNode(end, value);
        newNode->next = array[start].first;
        array[start].first = newNode;

        //adds node to adj list at end value
        newNode = new GraphNode(start, value);
        newNode->next = array[end].first;
        array[end].first = newNode;
    }
    ~Graph() {
        for (int i = 0; i < vertices; i++) {
            GraphNode* current = array[i].first;
            while (current != nullptr) {
                GraphNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }
};

//ds - choice of data structure, "fibonacci" or "pairing", otherwise error
void Prim(Graph* graph, string ds, Metrics* metrics = nullptr) {
    int vert = graph->vertices;//number of vertices
    vector<int> values(vert, INT_MAX);
    vector<int> final(vert, -1);


    if(ds == "fibonacci") {
        vector<FibNode*> fibNodeArray; //Fib heap node array. store node from insertKey opp.
        FibonacciHeap* fibHeap = new FibonacciHeap;
        fibNodeArray.resize(vert);

        FibNode* tempNode;
            for(int i = 1; i < vert; i++){
                values.at(i) = INT_MAX;
                final.at(i) = -1;
                tempNode = fibHeap->insert(INT_MAX);
                tempNode->num = i;
                fibNodeArray.at(i) = tempNode;
            }

                //Set first vertex to be 0
            values[0] = 0;        
            tempNode = fibHeap->insert(0);
            tempNode->num = 0;
            fibNodeArray[0] = tempNode;

            while(fibHeap->getMin() != nullptr){
                if(metrics) metrics->startExtractMin();
                FibNode* minNode = fibHeap->extractMin();
                if(metrics) metrics->stopExtractMin();
                int u = minNode->num;//min vertex number
                fibNodeArray.at(u) = nullptr;
                GraphNode* search = graph->array.at(u).first;

                while(search != nullptr){
                    int v = search->dest;

                    if(fibNodeArray.at(v) != nullptr && search->weight < values.at(v)){//if in the array
                        values.at(v) = search->weight;//set comparision values
                        final.at(v) = u;//set better edge
                        if(metrics) metrics->startDecreaseKey();
                        fibHeap->decreaseKey(fibNodeArray.at(v), search->weight);//set node in heap to new value
                        if(metrics) metrics->stopDecreaseKey();
                    }
                    search = search->next;
                }
            }

            // for(int i = 0; i < vert; i++){
            //     cout << i << " - " << final.at(i) << endl;
            // }
            delete fibHeap;
        
    }
    else if(ds == "pairing") {
        PairingHeap* pairHeap = new PairingHeap;
        vector<Node*> nodeArray; //Pairing heap node array. store node from insertKey opp.
        nodeArray.resize(vert);
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
        while(!pairHeap->isEmpty()){
            if(metrics) metrics->startExtractMin();
            newNode = pairHeap->extractMin(); //Pulls min from heap
            if(metrics) metrics->stopExtractMin();
            nodeArray[newNode->vertex] = nullptr;
            GraphNode* search = graph->array[newNode->vertex].first; //sets search as the first node in the minNode's adj list

            while(search != nullptr){
                int destination = search->dest;
                //If note extracted yet(if pointer in array = nullptr), 
                if(nodeArray[destination] != nullptr && search->weight < values[destination]){
                    values[destination] = search->weight; //set value to new value
                    final[destination] = newNode->vertex;  //set that destination will connect to vertex ( [i] -> v)
                    if(metrics) metrics->startDecreaseKey();
                    pairHeap->decreaseKey(nodeArray[destination], search->weight); //update key of pairing heap node
                    if(metrics) metrics->stopDecreaseKey();
                }
                if(search->next != nullptr){
                }
                search = search->next;
            }
        }

        //Outputs completed array
        // for(int i = 0; i < vert; i++){
        //     cout << i << " - " << final[i] << endl;
        // }
        delete pairHeap;
        return;

        }
    else {
        cout << "invalid value of ds, try again with \"fibonacci\" or \"pairing\"" << endl;
        return;
    }
    

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
    ofstream csvFile("prim_results.csv");
    csvFile << "n,total_mean,total_sd,decrease_mean,decrease_sd,extract_mean,extract_sd,ops_mean,ops_sd,heap_type,graph_type\n";

    for (int n = 10; n <= 1000; n *= 10) {  // small sizes first; scale up as needed

        for (auto graph_type : {"Sparse", "Dense"}) {

            cout << "====================================\n";
            cout << graph_type << " graph, size n = " << n << "\n";

            // Vectors to store per-run metrics
            vector<double> fib_total(RUNS), fib_dec(RUNS), fib_ex(RUNS);
            vector<int> fib_ops(RUNS);

            vector<double> pair_total(RUNS), pair_dec(RUNS), pair_ex(RUNS);
            vector<int> pair_ops(RUNS);

            for (int r = 0; r < RUNS; r++) {

                Graph* newGraph = new Graph(n);

                unsigned seed = 42 + r;
                mt19937 gen(seed);
                uniform_int_distribution<> vertexDistrib(0, n - 1);
                uniform_int_distribution<> weightDistrib(0, 1000);

                // Determine number of edges
                int numEdges;
                if (string(graph_type) == "Sparse") {
                    numEdges = n; // ~1 edge per vertex
                } else {
                    double density = 0.2; // 20% of all possible edges
                    numEdges = static_cast<int>(n * (n - 1) / 2 * density);
                }

                // Use a set to avoid duplicate edges
                set<pair<int,int>> addedEdges;

                // Ensure connectivity first (chain)
                for (int i = 0; i < n-1; i++) {
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
                Prim(newGraph, "fibonacci", &metrics);
                metrics.stopRun();

                fib_total[r] = metrics.totalRunTime();
                fib_dec[r]   = metrics.decreaseKeyTime();
                fib_ex[r]    = metrics.extractMinTime();
                fib_ops[r]   = metrics.heapOps();
                metrics.clear();

                // Pairing Heap
                metrics.startRun();
                Prim(newGraph, "pairing", &metrics);
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

            cout << "Prim (Fibonacci Heap) — Total runtime: " << total_mean << " s ± " << total_sd << " s\n";
            csvFile << n << "," << total_mean << "," << total_sd << ","
                    << dec_mean << "," << dec_sd << ","
                    << ex_mean << "," << ex_sd << ","
                    << ops_mean << "," << ops_sd << ",Fibonacci," << graph_type << "\n";

            // Pairing Heap metrics
            computeMeanSD(pair_total, mean, sd); total_mean = mean; total_sd = sd;
            computeMeanSD(pair_dec, mean, sd);   dec_mean = mean; dec_sd = sd;
            computeMeanSD(pair_ex, mean, sd);    ex_mean = mean; ex_sd = sd;
            computeMeanSD(pair_ops, mean, sd);   ops_mean = mean; ops_sd = sd;

            cout << "Prim (Pairing Heap) — Total runtime: " << total_mean << " s ± " << total_sd << " s\n";
            csvFile << n << "," << total_mean << "," << total_sd << ","
                    << dec_mean << "," << dec_sd << ","
                    << ex_mean << "," << ex_sd << ","
                    << ops_mean << "," << ops_sd << ",Pairing," << graph_type << "\n";
        }
    }

    csvFile.close();
    cout << "\nCSV results written to prim_results.csv\n";

    return 0;
}
