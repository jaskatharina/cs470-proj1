#include <iostream>
#include <random>
#include <chrono>

using namespace std;

class Node {
public:
    int key;
    Node* child = nullptr;
    //refers to next child on same level, if no such child is nullptr
    Node* next = nullptr;
    //leftmost child points to parent, otherwise refers to prev child on same level
    Node* prev = nullptr;
    Node(int k) {
        this->key = k;
    }
};

bool isEmpty(Node* node) {
    return node == nullptr;
}
//if prev points to node's parent, returns true
bool isLeftmostChild(Node* node) {
    if(node->prev != nullptr) {
        if(node->prev->child == node) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        cout << "Error: node->prev is nullptr";
        return false;
    }
}

Node* findLastChild(Node* parent) {
    Node* child = parent->child;
    //loop through to last child of parent 
    while(child != nullptr) {
        child = child->next;
    }
    return child;
}

Node* meld(Node* node1, Node* node2) {
    if(isEmpty(node1)) {
        return node2;
    }
    else if(isEmpty(node2)) {
        return node1;
    }
    //make node2 a child of node1
    else if(node1->key <= node2->key) {    
        if(!isEmpty(node1->child)) { //node2 is not the first child of node1
            node2->next = node1->child;
            node1->child->prev = node2;
        }
        node2->prev = node1;
        node1->child = node2;
        return node1;
    }
    //make node1 a child of node2
    else {
        if(!isEmpty(node2->child)) { //node1 is not the first child of node1
            node1->next = node2->child;
            node2->child->prev = node1;
        }
        node1->prev = node2;
        node2->child = node1;
        return node2;
    }
}

Node* mergePairs(Node* child) {
    if(isEmpty(child)) {
        return nullptr;
    }
    else if(isEmpty(child->next)) {
        return child;
    }
    else {
        return meld(meld(child, child->next), mergePairs(child->next->next));
    }
}


    


class PairingHeap {
public:
    Node* root = nullptr;

    Node* findMin() {
        return root;
    }
    
    Node* insert(int key) {
        Node* newNode = new Node(key);
        meld(this->root, newNode);
        return newNode;
    }

    void decreaseKey(Node* node, int newKey) {

        //remove subtree rooted at node
        if(node->prev == nullptr) {
            cout << "Error: calling decreaseKey on root";
        }
        else {
            if(isLeftmostChild(node)) {
                //prev refers to node's parent
                node->prev->child = node->next;
            }
            else {
                //prev refers to node's sibling
                node->prev->next = node->next;
            }  
        }
        node->key = newKey;
        meld(this->root, node);
    }

    Node* extractMin() {
        if(isEmpty(root)) {
            cout << "Error: extractMin called on empty heap";
        }
        else if(isEmpty(root->child)) {
            return root;
        }
        else {
            this->root = mergePairs(root->child);
        }
        
    }

};

int main() {

    int n = 100;
    int lower_bound = 0;
    int upper_bound = 65535;

    //init random number gen for testing
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    uniform_int_distribution<> distrib(0,65535);

    PairingHeap pHeap;

    for(int i = 0; i < n; i++) {
        pHeap.insert(distrib(gen));
    }
}