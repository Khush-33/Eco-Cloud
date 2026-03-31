# Phase 1: Method Reconstruction Sheet (D3)

## 1. System Inputs Required
- **Job Submissions:** Unmodified DL jobs (Dockerized/Singularity) assuming standard GPU execution. No model-specific configurations required.
- **Hardware Telemetry:** Real-time GPU power draw (via NVML), estimated CPU power (utilization-based model), static node power constants.
- **Progress Metrics:** Throughput (samples/second) or Progress (completed epochs/total epochs). Harvested passively from standard logs or plugin intercept.
- **Environment Context:** 48-hour Carbon Intensity Curve (CIC) dataset via regional API (e.g., ElectricityMaps) yielding `gCO2/kWh` per block.

## 2. Core Variables & Equations
- **Job Power Estimation:**  
  `P_job(t) = P_gpu(t) + P_cpu_model(t) + P_static`
  *Tracked discretely at 30-second steps.*
- **Carbon Footprint Accrual:**  
  `FOOTPRINT_job(T) = ∫[0 to T] P_job(t) * CIC(t) dt`
- **Energy Efficiency Metric:**  
  `EFFICIENCY_job(t) = ΔProgress_job(t) / ΔW_job(t)`  
  *(where W is accumulated energy in kWh)*
- **Efficiency Degradation Factor (The "Scalability" Checker):**  
  `DEGRADATION_job(t) = EFFICIENCY_job(t) / EFFICIENCY_job(t0)`  
  *Measures how well the job retains energy-to-progress efficiency as GPUs are added.*
- **Shifting Factor Calculation:**  
  Checks if current global CIC > Average CIC (`Ī`) and if `P_job > P_median`.  
  Rescales job power to `[1, µ]`. If the grid is dirty, high-power jobs get a large penalty multiplier. If the grid is clean, they get a small multiplier.
- **Final Priority Score:**  
  `PRIORITY_job = (FOOTPRINT_job / DEGRADATION_job) * SHIFTING_job`  
  *(Higher score = lower scheduling priority -> gets preempted)*

## 3. Data & Control Flow
### Data Flow
1. **Profiler Agent** parses NVML and stdout logs -> outputs `P_job`, `ΔW`, `ΔProgress`.
2. **Daemon** queries Carbon API -> outputs `CIC(t)`.
3. **Optimizers** synthesize metrics -> output `DEGRADATION_job`, `FOOTPRINT_job`, `SHIFTING_job`.
4. **MLFQ Engine** -> outputs updated Queue positions & allocations.

### Control Flow (MLFQ)
- **Job Arrival:** Job enters the **Upper Queue** with 1 GPU.
- **Scaling Phase (Upper Queue):** 
  - Iterates jobs FCFS. Checks: `DEGRADATION_job >= γ`?
  - If YES & Cluster Cap not hit -> Allocate +1 GPU.
  - If NO or Upper Queue limit hit (e.g., `θ = 30%` cluster capacity) -> Evict to **Lower Queue**.
- **Steady State / Shifting Phase (Lower Queue):**
  - Sorts jobs by `PRIORITY_job` (ascending).
  - Actively schedules lower-score jobs until 100% capacity hit.
  - Generates SIGTERM to preempt displaced jobs (120s checkpoint grace period).
- **Triggers:** Re-evaluated on active 30-minute *quanta* or passively on any job completion.

## 4. Hyperparameters and Knobs
| Knob | Function | Default in Paper | Note |
|:---|:---|:---|:---|
| `γ` (Gamma) | Degradation Tolerance | 0.9 (approx) | Avoids adding GPUs if efficiency drops by >10%. |
| `µ` (Mu) | Shifting Aggressiveness | 2.0 | Bounds the max power scale penalty; if `µ=1`, time shifting is disabled. |
| `θ` (Theta) | Upper Queue Capacity Cap | 30% | Prevents massive influx of new jobs from starving older jobs. |
| `Active Quantum`| MLFQ Re-eval Interval | 30 minutes | Balances shifting granularity against checkpoint/restart overheads. |