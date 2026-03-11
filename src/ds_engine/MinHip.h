#include <iostream>
#include "PCB.h"

using namespace std;

// Node to hold the process and its priority score
struct HeapNode {
    PCB* process;
    int eco_score; // Lower score means higher priority
};

class MinHeap {
private:
    HeapNode heap[100]; // Fixed size array (classic college style)
    int size;

    // Standard Heap Math
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return (2 * i) + 1; }
    int rightChild(int i) { return (2 * i) + 2; }

public:
    MinHeap() {
        size = 0;
    }

    // Insert new process
    void insert(PCB* p, int score) {
        if (size == 100) {
            cout << "Heap Overflow!" << endl;
            return;
        }

        // Insert at the very end of the array
        int index = size;
        heap[index].process = p;
        heap[index].eco_score = score;
        size++;

        // "Bubble Up" to fix Min-Heap property
        while (index != 0 && heap[parent(index)].eco_score > heap[index].eco_score) {
            // Swap child with parent
            HeapNode temp = heap[index];
            heap[index] = heap[parent(index)];
            heap[parent(index)] = temp;
            
            // Move up
            index = parent(index);
        }
    }

    // Remove and return the minimum element
    PCB* extractMin() {
        if (size <= 0) return NULL;
        if (size == 1) {
            size--;
            return heap[0].process;
        }

        // Store the root (minimum)
        PCB* root = heap[0].process;

        // Move the last element to the root
        heap[0] = heap[size - 1];
        size--;

        // "Heapify Down" (simplified iterative version)
        int index = 0;
        while (leftChild(index) < size) {
            int smallest = leftChild(index);
            
            // Check if right child exists and is smaller than left child
            if (rightChild(index) < size && heap[rightChild(index)].eco_score < heap[smallest].eco_score) {
                smallest = rightChild(index);
            }

            // If current is smaller than both children, we are done
            if (heap[index].eco_score <= heap[smallest].eco_score) {
                break;
            }

            // Swap current with the smallest child
            HeapNode temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;

            index = smallest;
        }

        return root;
    }
};