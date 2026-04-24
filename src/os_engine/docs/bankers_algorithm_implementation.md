# Banker's Algorithm Implementation for Eco-Cloud OS Engine

## Overview

Banker's Algorithm has been integrated into the Eco-Cloud OS kernel as a **deadlock avoidance mechanism** for safe energy resource allocation. Instead of a simple "grant-or-deny" approach, the system now performs sophisticated safety checks before allocating any energy resources.

## What is Banker's Algorithm?

Banker's Algorithm is a resource allocation strategy that prevents deadlock by:
1. **Tracking** resource requirements and allocations for each process
2. **Simulating** hypothetical allocations to check if they lead to a safe state
3. **Granting** resources only if the system remains in a safe state (can complete all processes)
4. **Denying** requests that would lead to an unsafe state (potential deadlock)

## Implementation Components

### File: `bankers_algorithm.h`

Contains the complete Banker's Algorithm implementation with:

- **Resource Types**: Solar, Grid, Battery power
- **State Structures**:
  - `available[]`: Free resources of each type
  - `maximum[]`: Maximum resource each process will ever need
  - `allocated[]`: Currently allocated resources
  - `need[]`: Remaining resources each process needs

- **Key Functions**:
  - `bankers_init()`: Initialize the algorithm state
  - `bankers_set_available()`: Set total available resources
  - `bankers_set_maximum()`: Define maximum requirements per process
  - `bankers_request_resources()`: Request allocation with safety check
  - `bankers_release_resources()`: Return resources when process completes
  - `bankers_is_safe()`: Verify if system is in safe state
  - `bankers_print_state()`: Debug output of algorithm state

### File: `os_core.c` (Modified)

Integration points:

1. **Initialization (main function)**:
   ```c
   bankers_init(&bankers_state, TOTAL_JOBS, MAX_RESOURCE_TYPES);
   bankers_set_available(&bankers_state, 100, 200, 50);  // Solar, Grid, Battery
   ```

2. **Resource Request (sim_request_resource)**:
   - Calls `bankers_request_resources()` instead of simple budget check
   - Spreads requests across multiple resource types (realistic eco-system)
   - Logs whether allocation is GRANTED (safe) or DENIED (unsafe)

3. **Resource Release (sim_release_resource)**:
   - Calls `bankers_release_resources()` when process terminates
   - Returns all allocated resources back to the pool

### File: `PCB.h` (Modified)

Enhanced Process Control Block with:
- `process_id`: Tracks which process in the algorithm state this job maps to

## How It Works in Practice

### Scenario Example

**Initial State:**
- Available: Solar=100W, Grid=200W, Battery=50W
- Each job's maximum need: Solar=20W, Grid=50W, Battery=10W

**Process 0 Allocates:**
1. Requests Solar=16W, Grid=50W, Battery=8W
2. Algorithm checks: Can remaining jobs complete with what's left?
3. **GRANTED** ✓ (Safe state maintained)

**Process 1 Allocates:**
1. Requests same amounts
2. Algorithm checks if all jobs can still complete
3. **GRANTED** ✓ (Still safe)

**If Request Would Break Safety:**
- Algorithm temporarily allocates resources
- Simulates resource release when each process completes
- If any process can't possibly complete → **DENIED** (Prevents deadlock)

## Benefits for Eco-Cloud

1. **Deadlock Prevention**: Prevents scenarios where multiple jobs can't complete due to resource contention
2. **Fair Allocation**: All jobs that can safely run are allocated resources
3. **System Stability**: Guarantees system won't enter circular wait condition
4. **Energy Optimization**: Prevents wasteful rejection of jobs by checking feasibility upfront
5. **Multi-Resource Tracking**: Handles multiple energy sources (solar, grid, battery) simultaneously

## Output Interpretation

### During Allocation:
```
[BANKER'S ALGORITHM] Checking safe allocation for Process X...
[HARDWARE] Access GRANTED (Safe State Maintained).
```

### During Release:
```
[BANKER'S ALGORITHM] Releasing allocated resources for Process X...
```

### Final Report:
Shows complete breakdown of all resource allocations and availability, proving all jobs completed successfully without deadlock.

## Compilation

```bash
gcc -pthread -o os_core os_core.c -Wall -Wextra
```

## Testing

Run the executable to see:
- Job creation and queuing
- Banker's Algorithm safety checks for each allocation
- Resource allocation decisions (GRANTED/DENIED)
- Process completion and resource release
- Final system state verification

All jobs should complete successfully as the system ensures safe allocations throughout execution.
