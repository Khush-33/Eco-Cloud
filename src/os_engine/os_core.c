// src/os_engine/os_core.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "PCB.h"

#define QUEUE_CAPACITY 5
#define TOTAL_JOBS 8
#define NUM_CORES 2

// --- SHARED RESOURCES ---
PCB ready_queue[QUEUE_CAPACITY];
int queue_front = 0;
int queue_rear = 0;
int jobs_produced = 0;
int jobs_consumed = 0;

// Eco-Cloud Tracking Metrics
int successful_jobs = 0;
int failed_jobs = 0;

// Shared Energy Budget (Cooperating Processes)
int global_energy_budget = 200; // Intentionally low (200W) so some jobs fail/deny

// --- CONCURRENCY PRIMITIVES ---
pthread_mutex_t queue_mutex;    // Protects the Ready Queue
pthread_mutex_t energy_mutex;   // Protects the Global Energy Budget
pthread_mutex_t print_mutex;    // Prevents terminal output from scrambling
sem_t empty_slots;              // Tracks empty slots in queue
sem_t full_slots;               // Tracks filled slots in queue

// --- SYSTEM CALL SIMULATION ---
void sim_request_resource(PCB* job, int watts_needed, int core_id) {
    job->mode = KERNEL_MODE; // Switch to Kernel Mode
    
    pthread_mutex_lock(&print_mutex);
    printf("      -> [SYSTEM CALL] Requesting %d Watts (Mode: KERNEL)\n", watts_needed);
    pthread_mutex_unlock(&print_mutex);
    
    // CRITICAL SECTION: Modifying shared energy
    pthread_mutex_lock(&energy_mutex);
    if (global_energy_budget >= watts_needed) {
        global_energy_budget -= watts_needed;
        job->energy_allocated = 1; // Success
        
        pthread_mutex_lock(&print_mutex);
        printf("      -> [HARDWARE] Access Granted. Remaining Grid Budget: %d W\n", global_energy_budget);
        pthread_mutex_unlock(&print_mutex);
    } else {
        job->energy_allocated = 0; // Failure
        
        pthread_mutex_lock(&print_mutex);
        printf("      -> [HARDWARE] DENIED! Power Grid Overload Prevention Triggered.\n");
        pthread_mutex_unlock(&print_mutex);
    }
    pthread_mutex_unlock(&energy_mutex);
    
    job->mode = USER_MODE; // Switch back to User Mode
}

// --- PRODUCER: JOB GENERATOR (ADMISSION CONTROL) ---
void* job_generator(void* arg) {
    for (int i = 1; i <= TOTAL_JOBS; i++) {
        PCB new_job;
        sprintf(new_job.job_id, "JOB_%03d", i);
        new_job.burst_time = (rand() % 3) + 1; // 1 to 3 seconds
        new_job.state = STATE_NEW;
        new_job.mode = USER_MODE;
        new_job.energy_allocated = 0; // Default

        sem_wait(&empty_slots);
        pthread_mutex_lock(&queue_mutex);
        
        // Transition: NEW -> READY
        new_job.state = STATE_READY;
        ready_queue[queue_rear] = new_job;
        queue_rear = (queue_rear + 1) % QUEUE_CAPACITY;
        
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&full_slots);
        
        // Cleanly print queue admission
        pthread_mutex_lock(&print_mutex);
        printf("\n[ADMISSION CONTROL] [+] %s Created & Queued.\n", new_job.job_id);
        printf("                    -> State Transition: [NEW] ->[%s]\n", getStateName(new_job.state));
        pthread_mutex_unlock(&print_mutex);
        
        sleep(1); // Delay between job arrivals
    }
    
    // DEADLOCK PREVENTION: Wake up any sleeping consumers so they can exit cleanly
    for(int i = 0; i < NUM_CORES; i++) {
        sem_post(&full_slots); 
    }
    
    return NULL;
}

// --- CONSUMER: CPU CORE ---
void* cpu_core(void* core_id) {
    int id = *((int*)core_id);
    
    while (1) {
        // Wait for a job to arrive (or for the Producer shutdown signal)
        sem_wait(&full_slots);
        
        pthread_mutex_lock(&queue_mutex);
        // Check if we were woken up just to shut down
        if (jobs_consumed >= TOTAL_JOBS) {
            pthread_mutex_unlock(&queue_mutex);
            break; // Exit the thread
        }
        
        // Extract Job
        PCB current_job = ready_queue[queue_front];
        queue_front = (queue_front + 1) % QUEUE_CAPACITY;
        jobs_consumed++;
        
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&empty_slots);

        // --- CONTEXT SWITCH & START LOGGING ---
        pthread_mutex_lock(&print_mutex);
        printf("\n[CORE %d] [*] DISPATCHING %s\n", id, current_job.job_id);
        printf("      -> Context Switching... (Loading Registers)\n");
        current_job.state = STATE_RUNNING;
        printf("      -> State Transition: [READY] -> [%s]\n", getStateName(current_job.state));
        pthread_mutex_unlock(&print_mutex);
        
        usleep(500000); // Context switch penalty (0.5 sec)

        // --- SYSTEM CALL FOR ENERGY ---
        sim_request_resource(&current_job, 50, id); 
        
        // --- EXECUTION OUTCOME ---
        if (current_job.energy_allocated == 1) {
            pthread_mutex_lock(&print_mutex);
            printf("      -> Executing payload for %d seconds...\n", current_job.burst_time);
            pthread_mutex_unlock(&print_mutex);
            
            sleep(current_job.burst_time); // Only execute if energy was granted
        } else {
            pthread_mutex_lock(&print_mutex);
            printf("      -> Aborting Execution due to lack of energy.\n");
            pthread_mutex_unlock(&print_mutex);
            
            usleep(200000); // Minor delay for abort process
        }
        
        // --- TERMINATION LOGGING & STATS ---
        current_job.state = STATE_TERMINATED;
        
        pthread_mutex_lock(&print_mutex);
        printf("[CORE %d] [-] FINISHED %s\n", id, current_job.job_id);
        printf("      -> State Transition: [RUNNING] -> [%s]\n", getStateName(current_job.state));
        
        if (current_job.energy_allocated == 1) {
            printf("      -> Final Outcome: SUCCESS (Energy Consumed)\n");
            successful_jobs++;
        } else {
            printf("      -> Final Outcome: FAILED  (Energy Denied)\n");
            failed_jobs++;
        }
        printf("--------------------------------------------------\n");
        pthread_mutex_unlock(&print_mutex);
    }
    return NULL;
}

// --- MAIN FUNCTION ---
int main() {
    printf("==================================================\n");
    printf(" ECO-CLOUD OS KERNEL \n");
    printf("==================================================\n");

    // Initialize Mutexes and Semaphores
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&energy_mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL); 
    
    sem_init(&empty_slots, 0, QUEUE_CAPACITY);
    sem_init(&full_slots, 0, 0);

    // Create Threads
    pthread_t producer_thread;
    pthread_t cpu_thread_1, cpu_thread_2;
    int core_1 = 1, core_2 = 2;

    pthread_create(&producer_thread, NULL, job_generator, NULL);
    pthread_create(&cpu_thread_1, NULL, cpu_core, &core_1);
    pthread_create(&cpu_thread_2, NULL, cpu_core, &core_2);

    // Wait for threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(cpu_thread_1, NULL);
    pthread_join(cpu_thread_2, NULL);

    // Cleanup
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&energy_mutex);
    pthread_mutex_destroy(&print_mutex);
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);

    // FINAL OUTPUT STATISTICS
    printf("\n==================================================\n");
    printf(" SYSTEM SHUTDOWN: All jobs processed.\n");
    printf("==================================================\n");
    printf(" EXECUTION SUMMARY:\n");
    printf(" -> Total Jobs Processed       : %d\n", TOTAL_JOBS);
    printf(" -> Successful Terminations    : %d\n", successful_jobs);
    printf(" -> Failed Terminations        : %d (Energy not available)\n", failed_jobs);
    printf(" -> Remaining Grid Budget      : %d W\n", global_energy_budget);
    printf("==================================================\n");

    return 0;
}