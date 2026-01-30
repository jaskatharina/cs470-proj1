#include <bits/stdc++.h>
using namespace std;

class MinHeap {
    vector<int> heap;   // heap array
    int num;

    int parent(int i) { return (i - 1) / 2; }
    int left(int i)   { return 2 * i + 1; }
    int right(int i)  { return 2 * i + 2; }

    void heapifyUp(int i) {
        while (i > 0 && heap[parent(i)] > heap[i]) {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    void heapifyDown(int i) {
        int n = heap.size();
        while (true) {
            int smallest = i;
            int l = left(i), r = right(i);

            if (l < n && heap[l] < heap[smallest]) smallest = l;
            if (r < n && heap[r] < heap[smallest]) smallest = r;

            if (smallest == i) break;
            swap(heap[i], heap[smallest]);
            i = smallest;
        }
    }

public:
    // Insert value, returns its index (handle)
    int insert(int val) {
        heap.push_back(val);
        int idx = heap.size() - 1;
        heapifyUp(idx);
        return idx;
    }

    int extractMin() {
        if (heap.empty()) throw runtime_error("Heap is empty");

        int root = heap[0];
        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty())
            heapifyDown(0);

        return root;
    }

    void decreaseKey(int index, int newVal) {
        if (index < 0 || index >= heap.size())
            throw runtime_error("Invalid index");

        if (newVal > heap[index])
            throw runtime_error("New value is greater than current value");

        heap[index] = newVal;
        heapifyUp(index);
    }

    int getMin() const {
        if (heap.empty()) throw runtime_error("Heap is empty");
        return heap[0];
    }

    bool empty() const {
        return heap.empty();
    }
};
