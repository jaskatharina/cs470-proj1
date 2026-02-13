#include <iostream>
#include <cmath>
#include <limits>
#include <vector>

using namespace std;

struct FibNode {
    int num;
    int key;                                            // the actual stored value
    int degree;                                         // the degree of the node
    bool mark;                                          // is it a parent of child loss (true if TRUE)
    FibNode *parent;                                    // pointer to parent
    FibNode *child;                                     // pointer to main child
    FibNode *left;                                      // pointer to node to the left
    FibNode *right;                                     // pointer to node to the right

    FibNode(int ky)                                     // basic declaration
        : key(ky), degree(0), mark(false),
          parent(nullptr), child(nullptr),
          left(this), right(this) {}
};

class FibonacciHeap {
private:
    FibNode* minNode;
    int n;

    void link(FibNode* y, FibNode* x) {
        // Remove y from root list
        y->left->right = y->right;              // Stitch it out of the list
        y->right->left = y->left;               // Stitch it out of the list

        // Make y a child of x
        y->parent = x;
        if (!x->child) {
            x->child = y;                       // Stitch it into a child
            y->left = y->right = y;             // Stitch the child into the list
        } else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        }

        x->degree++;
        y->mark = false;
    }

    void consolidate() {
        int D = static_cast<int>(log2(n)) + 2;
        vector<FibNode*> A(D, nullptr);

        vector<FibNode*> roots;
        FibNode* curr = minNode;
        if (curr) {
            do {
                roots.push_back(curr);
                curr = curr->right;
            } while (curr != minNode);
        }

        for (FibNode* w : roots) {
            FibNode* x = w;
            int d = x->degree;
            while (A[d]) {
                FibNode* y = A[d];
                if (x->key > y->key)
                    swap(x, y);
                link(y, x);
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
        }

        minNode = nullptr;
        for (FibNode* node : A) {
            if (node) {
                if (!minNode) {
                    minNode = node;
                    node->left = node->right = node;
                } else {
                    node->left = minNode;
                    node->right = minNode->right;
                    minNode->right->left = node;
                    minNode->right = node;
                    if (node->key < minNode->key)
                        minNode = node;
                }
            }
        }
    }

    void cut(FibNode* x, FibNode* y) {
        if (x->right == x)                  // if x has no children
            y->child = nullptr;             // set y's child to nothing
        else {                              
            x->right->left = x->left;       // stitch x out of the linked list layer
            x->left->right = x->right;      // stitch x out of the linked list layer
            if (y->child == x)              // if y's child is x
                y->child = x->right;        // make y's child point to x's right neighbor
        }

        y->degree--;                        // decrease y's degree, since x removed

        x->left = minNode;                  
        x->right = minNode->right;
        minNode->right->left = x;
        minNode->right = x;

        x->parent = nullptr;                // remove x's parent
        x->mark = false;                    // dont mark it for fixing
    }

    void cascadingCut(FibNode* y) {
        FibNode* z = y->parent;
        if (z) {
            if (!y->mark)
                y->mark = true;
            else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }

public:
    FibonacciHeap() : minNode(nullptr), n(0) {}

    bool isEmpty() {
        return this->minNode == nullptr;
    }

    FibNode* insert(int key) {
        FibNode* node = new FibNode(key);
        if (!minNode) {
            minNode = node;
        } else {
            node->left = minNode;
            node->right = minNode->right;
            minNode->right->left = node;
            minNode->right = node;
            if (key < minNode->key)
                minNode = node;
        }
        n++;
        return node;
    }

    //overloaded insert that sets num
    FibNode* insert(int key, int num) {
        FibNode* node = new FibNode(key);
        node->num = num;
        if (!minNode) {
            minNode = node;
        } else {
            node->left = minNode;
            node->right = minNode->right;
            minNode->right->left = node;
            minNode->right = node;
            if (key < minNode->key)
                minNode = node;
        }
        n++;
        return node;
    }

    FibNode* getMin() const {
        return minNode;
    }

    FibNode* extractMin() {
        FibNode* z = minNode;
        if (z) {
            if (z->child) {
                FibNode* c = z->child;
                do {
                    FibNode* next = c->right;
                    c->left = minNode;
                    c->right = minNode->right;
                    minNode->right->left = c;
                    minNode->right = c;
                    c->parent = nullptr;
                    c = next;
                } while (c != z->child);
            }

            z->left->right = z->right;
            z->right->left = z->left;

            if (z == z->right)
                minNode = nullptr;
            else {
                minNode = z->right;
                consolidate();
            }

            n--;
        }
        return z;
    }

    void decreaseKey(FibNode* x, int newKey) {
        if (newKey > x->key)    // if the key is bigger than x's key, what are you doing? return.
            return;

        x->key = newKey;                // set x's key to the new key
        FibNode* y = x->parent;         // make the new node x's parent

        if (y && x->key < y->key) {     // if x's Key is less than y's Key, and y exsists
            cut(x, y);                  // cut
            cascadingCut(y);            // cut fully
        }

        if (x->key < minNode->key) // If x's Key value is less than the MinNode's Key, make MinNode point to X
            minNode = x;
    }

    void deleteNode(FibNode* x) {
        decreaseKey(x, numeric_limits<int>::min());
        extractMin();
        delete x;
    }

    void increaseKey(FibNode* x, int newKey) {
        if (newKey < x->key)
            return;

        deleteNode(x);
        insert(newKey);
    }
};
