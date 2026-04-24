// src/os_engine/simulation_logger.h
// JSON Event Logger for Real-Time OS Engine Visualization

#ifndef SIMULATION_LOGGER_H
#define SIMULATION_LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define MAX_EVENTS 10000

typedef struct {
    long timestamp_ms;
    char event_type[64];
    char job_id[16];
    int core_id;
    char from_state[32];
    char to_state[32];
    char resource_type[32];
    int resource_amount;
    int available_solar;
    int available_grid;
    int available_battery;
    int allocated_solar;
    int allocated_grid;
    int allocated_battery;
    char mode[16];
    int process_id;
    char details[256];
} SimulationEvent;

typedef struct {
    SimulationEvent events[MAX_EVENTS];
    int event_count;
    pthread_mutex_t log_mutex;
    long start_time_ms;
    FILE* live_output;
} SimulationLogger;

SimulationLogger global_logger;

void logger_init() {
    global_logger.event_count = 0;
    pthread_mutex_init(&global_logger.log_mutex, NULL);
    global_logger.start_time_ms = 0;
    global_logger.live_output = fopen("sim_events_live.jsonl", "w");
}

long get_current_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    
    if (global_logger.start_time_ms == 0) {
        global_logger.start_time_ms = (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
        return 0;
    }
    
    long current_ms = (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
    return current_ms - global_logger.start_time_ms;
}

void log_event_internal(SimulationEvent* event) {
    pthread_mutex_lock(&global_logger.log_mutex);
    
    if (global_logger.event_count >= MAX_EVENTS) {
        pthread_mutex_unlock(&global_logger.log_mutex);
        return;
    }
    
    event->timestamp_ms = get_current_ms();
    memcpy(&global_logger.events[global_logger.event_count], event, sizeof(SimulationEvent));
    
    // Write to live JSONL file for streaming
    if (global_logger.live_output) {
        fprintf(global_logger.live_output, "{\"ts\":%ld,\"type\":\"%s\",\"job\":\"%s\",\"core\":%d,\"from\":\"%s\",\"to\":\"%s\",\"sol\":%d,\"grid\":%d,\"bat\":%d,\"asol\":%d,\"agrid\":%d,\"abat\":%d,\"mode\":\"%s\",\"pid\":%d}\n",
            event->timestamp_ms, event->event_type, event->job_id, event->core_id,
            event->from_state, event->to_state,
            event->available_solar, event->available_grid, event->available_battery,
            event->allocated_solar, event->allocated_grid, event->allocated_battery,
            event->mode, event->process_id);
        fflush(global_logger.live_output);
    }
    
    global_logger.event_count++;
    pthread_mutex_unlock(&global_logger.log_mutex);
}

void log_job_created(const char* job_id, int process_id) {
    SimulationEvent event = {0};
    strncpy(event.event_type, "JOB_CREATED", 63);
    strncpy(event.job_id, job_id, 15);
    strncpy(event.to_state, "NEW", 31);
    event.process_id = process_id;
    log_event_internal(&event);
}

void log_state_transition(const char* job_id, int core_id, const char* from_state, 
                          const char* to_state, const char* mode) {
    SimulationEvent event = {0};
    strncpy(event.event_type, "STATE_TRANSITION", 63);
    strncpy(event.job_id, job_id, 15);
    strncpy(event.from_state, from_state, 31);
    strncpy(event.to_state, to_state, 31);
    strncpy(event.mode, mode, 15);
    event.core_id = core_id;
    log_event_internal(&event);
}

void log_resource_request(const char* job_id, int process_id, int solar, int grid, int battery,
                         int avail_solar, int avail_grid, int avail_battery,
                         int alloc_solar, int alloc_grid, int alloc_battery, int granted) {
    SimulationEvent event = {0};
    strncpy(event.event_type, granted ? "RESOURCE_GRANTED" : "RESOURCE_DENIED", 63);
    strncpy(event.job_id, job_id, 15);
    event.allocated_solar = alloc_solar;
    event.allocated_grid = alloc_grid;
    event.allocated_battery = alloc_battery;
    event.available_solar = avail_solar;
    event.available_grid = avail_grid;
    event.available_battery = avail_battery;
    event.resource_amount = grid;
    event.process_id = process_id;
    log_event_internal(&event);
}

void log_resource_release(const char* job_id, int process_id, int avail_solar, int avail_grid, int avail_battery) {
    SimulationEvent event = {0};
    strncpy(event.event_type, "RESOURCE_RELEASED", 63);
    strncpy(event.job_id, job_id, 15);
    event.available_solar = avail_solar;
    event.available_grid = avail_grid;
    event.available_battery = avail_battery;
    event.process_id = process_id;
    log_event_internal(&event);
}

void log_mode_switch(const char* job_id, const char* mode) {
    SimulationEvent event = {0};
    strncpy(event.event_type, "MODE_SWITCH", 63);
    strncpy(event.job_id, job_id, 15);
    strncpy(event.mode, mode, 15);
    log_event_internal(&event);
}

void logger_write_json(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) return;
    
    fprintf(fp, "{\"events\":[");
    
    for (int i = 0; i < global_logger.event_count; i++) {
        SimulationEvent* e = &global_logger.events[i];
        fprintf(fp, "{\"ts\":%ld,\"type\":\"%s\",\"job\":\"%s\",\"core\":%d,\"from\":\"%s\",\"to\":\"%s\",\"sol\":%d,\"grid\":%d,\"bat\":%d,\"asol\":%d,\"agrid\":%d,\"abat\":%d,\"mode\":\"%s\",\"pid\":%d}",
            e->timestamp_ms, e->event_type, e->job_id, e->core_id,
            e->from_state, e->to_state,
            e->available_solar, e->available_grid, e->available_battery,
            e->allocated_solar, e->allocated_grid, e->allocated_battery,
            e->mode, e->process_id);
        if (i < global_logger.event_count - 1) fprintf(fp, ",");
    }
    
    fprintf(fp, "],\"count\":%d}", global_logger.event_count);
    fclose(fp);
}

void logger_cleanup() {
    if (global_logger.live_output) {
        fclose(global_logger.live_output);
    }
}

#endif
