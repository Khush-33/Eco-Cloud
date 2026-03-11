#pragma once
#include <iostream>
#include <string>
using namespace std;

struct PCB {
    string job_id;   
    int burst_time;
    int memory_req;
    
    // CONSTRUCTOR: Expecting exactly 3 arguments (string, int, int)
    PCB(string id, int burst, int mem) {
        job_id = id;
        burst_time = burst;
        memory_req = mem;
    }

    void printDetails() {
        cout << "[Job ID: " << job_id 
             << " | Burst: " << burst_time 
             << " | Mem: " << memory_req << "MB]" << endl;
    }
};