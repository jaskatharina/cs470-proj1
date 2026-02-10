#ifndef PAIRINGHEAP_H
#define PAIRINGHEAP_H

#include <iostream>
#include <vector>


class Node {
public:
    int key;
    int vertex;
    Node* child = nullptr;
    //refers to next child on same level, if no such child is nullptr
    Node* next = nullptr;
    //leftmost child points to parent, otherwise refers to prev child on same level
    Node* prev = nullptr;
    Node(int k) {
        this->key = k;
    }
    Node(int k, int v) {
        this->key = k;
        this->vertex = v;
    }
};

//nodes are less than or equal to their children, and a node can have any number of children
//a parent only points to one of its children
//children point to their adjacent siblings, or to their parent, in the case of the leftmost child

class PairingHeap {
public:
    Node* root = nullptr;

    //returns the node with the minimum key, which is the root
    Node* findMin() {
        return root;
    }

    //checks if a pointer to a node is empty by comparing with null
    bool isEmpty(Node* node) {
        return node == nullptr;
    }

    //checks if heap is empty
    bool isEmpty() {
        return isEmpty(this->root);
    }

    //if prev points to node's parent, returns true
    //if prev points to node's sibling, returns false
    bool isLeftmostChild(Node* node) {
        if(node->prev != nullptr) {
            if(node->prev->child == node) {
                return true;
            }
            else {
                return false;
            }
        }
        else { //this should only be true for the root and this function should never be called on the root
            std::cout << "Error: node->prev is nullptr";
            return false;
        }
    }

    //combines two trees together by making the tree with the bigger root a child of the other tree
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
            if(!isEmpty(node2->child)) { //node1 is not the first child of node2
                node1->next = node2->child;
                node2->child->prev = node1;
            }
            node1->prev = node2;
            node2->child = node1;
            return node2;
        }
    }

    
    
    //creates a new node, adds it to the heap, and returns it
    Node* insert(int key) {
        Node* newNode = new Node(key);
        this->root = meld(this->root, newNode);
        return newNode;
    }

    //overloaded insert for use as priority queue
    Node* insert(int key, int vertex) {
        Node* newNode = new Node(key, vertex);
        this->root = meld(this->root, newNode);
        return newNode;
    }

    //extracts the node, replaces the key, and melds it back onto the heap
    void decreaseKey(Node* node, int newKey) {

        //if node is root just update its key
        if(isEmpty(node->prev)) {
            node->key = newKey;
            return;
        }
        else {
            Node* child = node->child;
            //replace node with child, make next child the child's child
            if(!isEmpty(child)) {
                child->child = child->next;
                child->prev = node->prev;
                child->next = node->next;
            }
            if(isLeftmostChild(node)) {
                //prev refers to node's parent
                node->prev->child = child;
            }
            else {
                //prev refers to node's sibling
                node->prev->next = child;
            }
            if(!isEmpty(node->next)) {
                node->next->prev = child;
            }
            node->prev = nullptr;
            node->child = nullptr;
            node->next = nullptr;
            node->key = newKey;
            this->root = meld(this->root, node);
        }
    }

    Node* extractMin() {
        if(isEmpty(this->root)) {
            std::cout << "Error: extractMin called on empty heap";
            return nullptr;
        }
        else {
            Node* r = this->root;
            if(isEmpty(this->root->child)) {
                this->root = nullptr;
            }
            else {
                //reset child's prev pointer so it no longer points to removed root
                this->root->child->prev = nullptr;
                //remove the root by merging root's children
                this->root = mergePairs(this->root->child);
            }
            return r;
        }
    }
    
    //recursively merge the root's children after removing it
    //merges the first two nodes in the list
    Node* mergePairs(Node* child) {
        if(isEmpty(child)) {
            return nullptr;
        }
        else if(isEmpty(child->next)) {
            return child;
        }
        else {
            Node* nextPair = child->next->next;
            Node* second = child->next;

            //disconnect each subheap from its siblings
            child->next = nullptr;
            child->prev = nullptr;
            second->next = nullptr;
            second->prev = nullptr;

            Node* melded = meld(child, second);
            return meld(melded, mergePairs(nextPair));
        }
    }




    //this print function is kind of a mess, but it at least reveals the heap's structure
    //each printed key with a slash on the line right below it is a parent
    //the children of the leftmost parent will be printed first
    //example: the fifth parent with a slash under it on a line has its children printed in the fifth group on the line below it

    
    void printHeap(std::vector<Node*> parents) { //parents should initially be empty

        if(parents.size() == 0) { //initial stack frame, prints the root and recurses with it as only parent
            std::cout << this->root->key << " | \n";
            std::cout << "/ \n";
            if(isEmpty(this->root->child)) {
                return;
            }
            else {
                parents.push_back(this->root);
                printHeap(parents);
            }
        }
        else { //recursive calls
            std::vector<Node*> new_parents;
            std::string line = "";
            std::string ptrs = "";
            for(int i = 0; i < parents.size(); i++) { //loop through parents
                Node* curr = parents.at(i)->child;
                while(!isEmpty(curr->next)) { //loop through all children of parent and print them
                    
                    int before = line.length();
                    line += std::to_string(curr->key) + " - ";
                    int diff = line.length() - before;
                    if(!isEmpty(curr->child)) {
                        ptrs += "/";
                        diff--;
                        new_parents.push_back(curr);
                    }
                    for(int j = 0; j < diff; j++) {
                        ptrs += " ";
                    }
                    curr = curr->next;
                }
                int before = line.length();
                line += std::to_string(curr->key) + " | ";
                int diff = line.length() - before;
                if(!isEmpty(curr->child)) {
                    ptrs += "/";
                    diff--;
                    new_parents.push_back(curr);     
                }
                for(int j = 0; j < diff; j++) {
                    ptrs += " ";
                }
            }
            std::cout << line << "\n";
            std::cout << ptrs << "\n";
            if(new_parents.size() == 0) { //base case, no more children to print
                return;
            }
            else { //otherwise recurse with parents to print their children
                printHeap(new_parents);
            }
        }
    }
};

#endif