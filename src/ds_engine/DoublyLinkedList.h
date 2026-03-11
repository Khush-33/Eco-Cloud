#include <iostream>
#include "PCB.h"
using namespace std;

// Standard Node Structure
struct Node {
    PCB* process;
    Node* next;
    Node* prev;
};

class DoublyLinkedList {
private:
    Node* head;
    Node* tail;

public:
    // Constructor initializes pointers to NULL
    DoublyLinkedList() {
        head = NULL;
        tail = NULL;
    }

    // Standard textbook Insert at End
    void insertAtEnd(PCB* p) {
        Node* newNode = new Node();
        newNode->process = p;
        newNode->next = NULL;
        newNode->prev = NULL;

        // If list is empty
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    // Standard textbook Delete from Front
    PCB* deleteFromFront() {
        if (head == NULL) {
            cout << "Queue is Empty!" << endl;
            return NULL;
        }

        Node* temp = head;
        PCB* data = temp->process;

        head = head->next;

        if (head != NULL) {
            head->prev = NULL;
        } else {
            tail = NULL; // List became empty
        }

        delete temp;
        return data;
    }
};