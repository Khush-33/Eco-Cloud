#include <iostream>
#include <string>
#include "PCB.h"

using namespace std;

struct TrieNode {
    TrieNode* children[26]; // 26 letters of alphabet
    bool isEndOfWord;
    PCB* process;           // Pointer to the actual PCB
};

class Trie {
private:
    TrieNode* root;

    // Helper to create a new empty node
    TrieNode* getNode() {
        TrieNode* newNode = new TrieNode();
        newNode->isEndOfWord = false;
        newNode->process = NULL;
        for (int i = 0; i < 26; i++) {
            newNode->children[i] = NULL;
        }
        return newNode;
    }

public:
    Trie() {
        root = getNode();
    }

    // Standard Trie Insert
    void insert(string jobId, PCB* p) {
        TrieNode* current = root;

        for (int i = 0; i < jobId.length(); i++) {
            int index = jobId[i] - 'a'; // Convert 'a'-'z' to 0-25

            // If path doesn't exist, create it
            if (current->children[index] == NULL) {
                current->children[index] = getNode();
            }
            
            // Move down the tree
            current = current->children[index];
        }

        // Mark the end of the ID and link the process
        current->isEndOfWord = true;
        current->process = p;
    }

    // Standard Trie Search
    PCB* search(string jobId) {
        TrieNode* current = root;

        for (int i = 0; i < jobId.length(); i++) {
            int index = jobId[i] - 'a';

            // If path breaks, job ID doesn't exist
            if (current->children[index] == NULL) {
                return NULL; 
            }
            current = current->children[index];
        }

        // If we reached the end and it's a valid word, return the PCB
        if (current != NULL && current->isEndOfWord == true) {
            return current->process;
        }

        return NULL;
    }
};