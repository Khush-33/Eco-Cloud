#include <iostream>
#include "PCB.h"
#include "DoublyLinkedList.h"
#include "MinHip.h"
#include "JobTrie.h"
#include "JobCorrector.h" 

using namespace std;

int main() {
    cout << "\n==================================================" << endl;
    cout << "   ECO-CLOUD: DATA STRUCTURE TERMINAL TEST" << endl;
    cout << "==================================================\n" << endl;

    PCB* p1 = new PCB("joba", 10, 256);
    PCB* p2 = new PCB("jobb", 5, 128);
    PCB* p3 = new PCB("jobc", 20, 512);

    cout << ">>> 1. TESTING DOUBLY LINKED LIST (FCFS Queue) <<<" << endl;
    DoublyLinkedList queue;
    queue.insertAtEnd(p1);
    queue.insertAtEnd(p2);
    queue.insertAtEnd(p3);
    
    PCB* currentJob = queue.deleteFromFront();
    while (currentJob != NULL) {
        cout << "   Dequeued: ";
        currentJob->printDetails();
        currentJob = queue.deleteFromFront(); 
    }

    cout << "\n>>> 2. TESTING MIN-HEAP (Eco-Aware Priority Scheduler) <<<" << endl;
    MinHeap ecoScheduler;
    ecoScheduler.insert(p1, 80); 
    ecoScheduler.insert(p2, 10); 
    ecoScheduler.insert(p3, 45); 

    PCB* scheduledJob = ecoScheduler.extractMin();
    while (scheduledJob != NULL) {
        cout << "   Scheduled: ";
        scheduledJob->printDetails();
        scheduledJob = ecoScheduler.extractMin();
    }

    cout << "\n>>> 3. TESTING TRIE (Fast Job ID Search) <<<" << endl;
    Trie jobDatabase;
    jobDatabase.insert(p1->job_id, p1);
    jobDatabase.insert(p2->job_id, p2);
    jobDatabase.insert(p3->job_id, p3);

    cout << "Searching for 'jobb': FOUND! -> ";
    PCB* search1 = jobDatabase.search("jobb");
    if (search1 != NULL) search1->printDetails();

    cout << "Searching for 'jobx' (Typo!): ";
    PCB* search2 = jobDatabase.search("jobx");
    if (search2 == NULL) cout << "Not Found." << endl;

    // ---------------------------------------------------------
    // NEW FEATURE TEST
    // ---------------------------------------------------------
    cout << "\n>>> 4. TESTING LEVENSHTEIN DISTANCE (Job Auto-Corrector) <<<" << endl;
    JobCorrector corrector;
    
    // Create an array of our active jobs to check against
    string activeJobs[3] = {p1->job_id, p2->job_id, p3->job_id};
    
    // Typo 1: Extra letter (joobc instead of jobc)
    string typo1 = "joobc";
    cout << "User searched for: '" << typo1 << "' -> Not Found." << endl;
    cout << "   Did you mean: '" << corrector.suggestCorrection(typo1, activeJobs, 3) << "'?" << endl;

    // Typo 2: Wrong letter (jobv instead of jobb)
    string typo2 = "jobv";
    cout << "\nUser searched for: '" << typo2 << "' -> Not Found." << endl;
    cout << "   Did you mean: '" << corrector.suggestCorrection(typo2, activeJobs, 3) << "'?" << endl;
    
    // Typo 3: Completely wrong (database)
    string typo3 = "database";
    cout << "\nUser searched for: '" << typo3 << "' -> Not Found." << endl;
    cout << "   Did you mean: '" << corrector.suggestCorrection(typo3, activeJobs, 3) << "'?" << endl;

    cout << "\n==================================================" << endl;
    cout << "               ALL TESTS COMPLETED                " << endl;
    cout << "==================================================\n" << endl;

    delete p1; delete p2; delete p3;
    return 0;
}