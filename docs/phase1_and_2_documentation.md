# Phase 1: One-Page Paper Summary
**Base Paper:** GREEN: Carbon-efficient Resource Scheduling for Machine Learning Clusters (NSDI 2025)

## 1. Problem Statement
Machine Learning (ML) workloads dictate a fast-growing segment of global energy consumption. Multi-tenant ML-as-a-Service (MLaaS) clusters traditionally schedule jobs to minimize Job Completion Time (JCT). However, this pure speed-centric focus ignores the critical temporal variabilities of the energy grid's **carbon intensity** (emissions per kWh) and the variations in individual jobs' **energy scalability**. Consequently, existing cluster scheduling policies implicitly inflate the cluster-wide carbon footprint. 

## 2. Why Existing Schedulers are Insufficient
- **Energy-unaware scheduling:** State-of-the-art schedulers (e.g., Pollux, Tiresias, Optimus) scale jobs to optimize throughput without tracking whether adding GPUs causes huge power spikes with diminishing returns in training progress.
- **Micro-focused energy optimization:** Schedulers like Zeus optimize energy versus JCT strictly at the *single-job level* and overlook cluster-wide job placement under static capacity constraints.
- **Naïve carbon-aware approaches:** Existing carbon-aware systems (like CICS, EcoVisor, GAIA) either blindly throttle cluster capacity during high-carbon periods (leaving multi-tenant cluster resources severely underutilized and hurting JCT) or assume infinite elastic cloud resources.

## 3. Core Method Design (GREEN)
GREEN acts as a *model-agnostic, capacity-constrained cluster scheduler* that leverages temporal job elasticity to optimize carbon efficiency while barely impacting overall JCT. The design revolves around three components:

1. **Carbon Tracker:** Uses a discretized power estimation model measuring GPU, estimated CPU, and static power. It correlates this with the diurnal Carbon Intensity Curve (CIC) to profile a running job's real-time footprint.
2. **Two Complementary Optimizers:**
   - **Energy Efficiency (EE) Optimizer:** Profiles jobs dynamically and scales resources (adds GPUs). The metric used is the job's *Energy Efficiency Degradation Factor*, keeping track of whether granting more GPUs continues to yield efficient progress relative to energy drawn. Jobs scaling poorly are capped.
   - **Carbon Footprint (CF) Optimizer:** Calculates a modified Priority Score = `(Footprint / Degradation) * Shifting_Factor`. A dynamic shifting factor delays high-power jobs to "greener" (low carbon intensity) hours.
3. **Multilevel Feedback Queue (MLFQ) Scheduler:**
   - **Upper Queue:** Accommodates new jobs and uses the EE Optimizer to scale resources out until the job stops scaling efficiently or a capacity cap (e.g., 30%) is reached.
   - **Lower Queue:** Manages mature jobs using the CF Optimizer, swapping high-power and low-power jobs depending on real-time grid carbon intensity. It limits preemption intervals to 30 minutes to reduce checkpointing overhead.

## 4. Main Results
- **Carbon Reduction:** Achieves up to **41.2%** reduction in total cluster carbon footprint (average of 32.2% across four distinct geographical regions) compared to Tiresias.
- **Power Draw:** Reduces peak power usage by roughly **12%**, effectively dropping peak capacity requirements.
- **JCT Trade-off:** Incurs only a **3.6% to 5.9%** increase in average JCT, and a **5.1% to 7.1%** increase in tail JCT against Tiresias.
- **Against Advanced Baselines:** Over Pollux, it achieves a **23.9%** average carbon reduction (with a slightly higher JCT trade-off due to Pollux’s non-model-agnostic hyperparameter tuning advantages).

## 5. Main Limitations
- **Geographical and Temporal Dependence:** Benefits are severely constrained in regions with persistently low or low-variance carbon intensity (e.g., Sweden, where absolute reductions are marginal).
- **Latency-Sensitive Jobs:** Relies inherently on preemption and shifting, meaning it strongly disadvantages user-facing inference tasks or rigid-deadline jobs.
- **Scope of Carbon Tracking:** Excludes *embodied carbon* (hardware lifecycle emissions) and infrastructure PUE from the optimization scope.
- **Congestion Starvation:** Despite Upper Queue limits, severe cluster load could exacerbate tail JCT for large, high-power jobs stalled during dirty hours due to the LAS (Least-Attained-Service) behavior.
# Phase 1: Claim-Evidence Table (D2)

| Claim ID | Claim | Metric | Baseline | Dataset/Workload | Evidence Location | Confidence | Notes |
|:---|:---|:---|:---|:---|:---|:---|:---|
| **C1** | Cluster-wide carbon footprint reduction up to 41.2% (32.2% avg across 4 regions). | Total cluster carbon footprint (kg CO2) | Tiresias, EcoVisor, GAIA | 791 jobs from University production cluster (ResNet, GPT-2, etc.); Replayed over 4 regions' CIC | Figure 8 (a-d), Section 8.2 | High | Reduction directly correlated with regional carbon variance; maximum savings in Poland/UK (high peaks). |
| **C2** | Reduction of carbon by 21% without temporal shifting (GREENns variant). | Total cluster carbon footprint (kg CO2) | Tiresias | 791 jobs testbed workload | Figure 8 & Section 8.2.1 | High | Proves the impact of just the Energy Efficiency setup (scalability throttling). |
| **C3** | 12% peak power reduction and 25% total energy consumption decrease. | Peak Power (kW) & Total Energy (kWh) | Tiresias | 791 jobs testbed workload (UK CIC trace) | Figure 9b (Cluster Power timeline) & Sec 8.2.1 | High | Crucial for limiting cluster provisioning limits. |
| **C4** | Comparable JCT to best baselines: 3.6 - 5.9% average JCT tradeoff. | Average Job Completion Time (hours) | Tiresias | 791 jobs testbed workload | Figure 8, Section 8.2.2 | High | Tradeoff predominantly paid by large/medium jobs (up to 6.9% increase); extra small jobs actually see -0.5% JCT. |
| **C5** | GREEN outperforms Pollux significantly in carbon metrics (23.9% avg reduction). | Carbon Footprint (kg CO2) | Pollux | Subset of 150 structurally compatible testbed jobs | Figure 11 (a-d), Section 8.2.3 | Medium | Due to Pollux's hyperparameter coupling, workload size was dropped to 150 jobs, narrowing the comparison. |
| **C6** | GREEN sustains 31.6% footprint reduction in year-long trace scaling. | Simulation Carbon Footprint | Tiresias | Year-long simulator (Alibaba trace, 146,000 jobs) | Figure 12 (Simulator results) | Medium-High | Validates adaptability across extreme seasonal variations. |
| **C7** | GREEN reduces JCT compared to carbon-aware platforms (EcoVisor, GAIA) by 25.2%. | Average JCT (hours) | GAIA, EcoVisor | 791 jobs testbed workload | Section 8.2.1 | High | Proves EcoVisor/GAIA's idle-waiting strategy fails badly in heavily congested multi-tenant setups. |
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
# Phase 1: Evaluation Reconstruction Sheet (D4)

## 1. Testbed and Cluster Setup
- **Hardware:** 32 Compute Nodes.
- **Node Specs:** 2x Nvidia RTX 3090 GPUs, 20 CPU cores, 64GB RAM per node.
- **Networking:** Mellanox ConnectX5 NIC.
- **Environment:** Ubuntu 18.04, CUDA v11.1, Docker-managed containers.
- **Manager:** Slurm (used via custom plugin for the GREEN scheduler).

## 2. Baselines Configuration
- **Tiresias:** Standard Least-Attained-Service (LAS) protocol. Used as the principal JCT baseline. Fixed GPUs per job (preset to best energy-efficient ratio based on GREEN's profiling).
- **GAIA (Carbon-Time):** Delays start to minimize carbon. Delay window `W` capped at 12 hours.
- **EcoVisor:** Threshold-based stalling. Jobs queued if CIC > 10th percentile of active region's CIC profile.
- **Pollux:** Advanced auto-scaling baseline. Mutates learning rate & batch size.
- **Zeus + Tiresias:** Combines Zeus’s single-job GPU power limit tuning with Tiresias's cluster LAS allocation.

## 3. Workload and Trace Design
- **Primary Trace (Testbed):** 791 real-world MLaaS jobs traced over 24 hrs from an academic cluster. Models: ResNet-50, ShuffleNet, DeepSpeech2, BERT, GPT-2. Job durations span 1 minute to 37 hours.
- **Filtered Trace (Ext. Baselines):** Only 150 jobs used when testing Pollux/Zeus since 80% of jobs had incompatible scaling/hyperparameter logic or non-standard architectures.
- **Simulator Trace:** 1-year scale up. 1 Minute granularity ticks. 146,000 jobs derived by sampling Alibaba cluster data over 365 iterations.

## 4. Metrics and Definitions
- **Carbon Footprint:** `kg CO2` (Cumulative integral of power * carbon intensity profile from distinct grids: UK, CA, Sweden, Poland).
- **Average JCT:** Job Completion Time measured from *submission to final completion*, encompassing queue waits, checkpoint saves, execution time, and preemption halts.
- **Tail JCT:** Typically P95 or P99 completion delay for long-running payloads.
- **Power Usage:** Real-time kW draw capturing active and dormant states across the cluster.

## 5. Ablations and Sensitivity Configurations
- **GREENns (No Shifting):** Set `µ=1` in the shifting equation to isolate the impact of *just* the Upper Queue Energy Efficiency scaling, keeping Shifting temporal re-queueing dormant.
- **Parameter `µ` Sweep:** Tested ranges from 1 to 3 to plot JCT vs. Carbon trade-off points. Hit sweet spot at `2`.
- **Preemption Interval Sweep:** Tested 10 mins, 30 mins, 60 mins. Demonstrated 30-min yields optimal overhead/adaptability balance.
# Phase 1: Limitations and Opportunities Log (D5)

## 1. Explicit Limitations (Declared by Authors)
1. **Low-Variance/Low-Carbon Geographies:** In regions like Sweden with flat, heavily renewable power profiles, the math defaults to nearly zero absolute gain, introducing unneeded preemption overheads for temporal shifting.
2. **Congestion-Induced Starvation:** The `LAS` (Least-Attained-Service) behavior built into the Carbon Factor formula may excessively stall large background jobs during heavy, sustained burst submissions (despite Upper queue partitioning thresholds).
3. **Forecasting Vulnerability:** Performance fundamentally assumes access to highly accurate predictive Carbon Intensity models (like day-ahead grids). Error spikes in CIC models inherently corrupt dispatch priorities.
4. **Tolerance Restrictions:** Entirely unsuitable for latency-sensitive applications like user-facing LLM inference, as it inherently stops and preempts execution indefinitely.

## 2. Implicit Limitations (Extracted via Critical Thinking)
1. **Measurement Purity Tradeoffs:** The reliance on `NVML` and software modeling for CPU power is notoriously imprecise. Spiky thermal behaviors could manipulate the priority quotient inadvertently compared to physical PDU telemetry.
2. **Small Job Overhead Parity:** Checkpointing and restarting a container every 30 minutes adds fixed penalty blocks (120s max). For volatile short jobs (lasting 30-45 mins), this represents a disproportionate tax relative to total emissions salvaged.
3. **Rigid Resource Binning:** Only treats scaling linearly by discrete GPU count. Does not interoperate natively with fractional GPU partitioning algorithms (like NVIDIA MIG) without modifications.
4. **Lack of User Fairness Controls:** Relies purely on carbon-footprint allocation balance. Users running fundamentally inefficient models monopolize preemptions from users developing highly optimized pipelines. No multi-tier hierarchical fairness boundaries (e.g. by Department/Project budget) are respected.
5. **No Thermal/PUE Awareness:** Ignores data-center specific PUE or server heat map data. Moving a high-power job to a clean time might inadvertently create a thermal hotspot requiring extreme HVAC operation, offsetting the grid's green advantage.

## 3. Opportunities & Potential Extensions Bank
- **[O1] Forecast-Noise Resilience (Robustness):** Introduce confidence intervals in the priority equation that degrade shifting aggressiveness if the carbon forecast drops below a reliability threshold.
- **[O2] Hybrid Inference-Training Pipelines (Systems):** Adapt MLFQ logic to allow strict pinning of latency-sensitive jobs while dynamically shifting *only* the flexible training/batch payloads around them using remaining capacity margins.
- **[O3] Spatial (Cross-Cluster) Load Shifting (Algorithmic):** Extend the CF optimizer to migrate workloads geographically across interlinked data centers based on respective instantaneous CIC deltas, rather than merely temporal deferral.
- **[O4] Embodied-Carbon Awareness (Conceptual Constraints):** Implement a carbon tracking equation that factors in server lifecycle aging metrics to favor execution on depreciated vs. newly-provisioned hardware instances.
- **[O5] Departmental/Budget-Based Fairness (Scheduling):** Enhance the MLFQ priority metric to respect user group quotas (e.g., Fair-Share scheduling combined with Eco-metrics), ensuring a single department can't be perpetually stalled purely based on their hardware draw patterns.
- **[O6] Checkpoint-Aware Penalty Avoidance (Systems):** Dynamically adjust the 30-minute re-evaluation frequency *per-job* based on precisely observed checkpoint generation costs rather than globally enforcing it.
# Phase 2: Gap, Questions, and Hypotheses (D6 - D10)

## D6: Gap Statement Set

### Candidate Gaps
1. **Candidate 1 (Forecasting Resilience):** While GREEN optimizes temporal shifting under the assumption of perfect real-time carbon intensity (CIC) forecasts, it does not securely handle real-world forecast noise or abrupt anomalies, which matters because inaccurate predictions can invert preemption priorities and actually accidentally increase structural grid emissions.
2. **Candidate 2 (Fairness & Priority):** While GREEN calculates cluster priority uniformly as a factor of raw node consumption and pure scaling degradation, it fails to encode administrative hierarchy or organizational fair-share bounds, which causes heavily parameterized research groups to incidentally throttle lean groups entirely out of prime operational windows.
3. **Candidate 3 (Latency vs. Flexibility Hybridization):** While GREEN explicitly disregards delay-sensitive (inference/serving) operations due to its hard MLFQ halt/shift mechanics, it fails to schedule environments that intrinsically mix micro-batch inference with long-tail batch training under fixed capacities.

### Final Selected Primary Gap (for Eco-Cloud Project)
**"While GREEN relies on rigid, formulaic heuristics (like MLFQ prioritization and piecewise shift functions) assuming perfect grid intensity knowledge, it degrades unpredictably under real-world Carbon Forecast noise and static thresholds. Our Eco-Cloud architecture abandons obsolete heuristic rule-engines, instead applying an end-to-end Deep Reinforcement Learning (DRL) approach to inherently learn carbon-aware scheduling policies."** 

---

## D7: Research Question Set

- **RQ1 (Primary):** Can a Deep Reinforcement Learning (DRL) agent completely replace static scheduling heuristics to dynamically optimize the trade-off between cluster carbon footprint and Job Completion Time (JCT)?
- **RQ2 (Secondary/Systems):** Removed to narrow scope and focus exclusively on the primary DRL agent\'s performance.
- **RQ3 (Secondary/Algorithmic):** How robust is a fully ML-driven scheduling pipeline against inherent noise in carbon grid forecasts compared to strictly deterministic, equation-based schedulers like GREEN?

---

## D8: Contribution Lock

We commit strictly to the following single, intense ML-focused contribution to guarantee project focus:

1. **Contribution 1: DRL-Based Carbon-Aware Scheduler (Algorithm):** 
   - A Deep Reinforcement Learning scheduling agent trained to ingest state configurations (cluster capacity, real-time noisy CIC, job queues) and output allocation actions, fully eliminating the need for hard-coded MLFQ rules or priority formulas.
   - *Expected Gain:* Superior adaptability to grid noise and complex job profiles without relying on manually tuned thresholds (like the arbitrary `µ=2` parameter in GREEN).
   - *Proof:* Simulation of the Alibaba trace using an OpenAI Gym-style environment evaluated against the deterministic GREEN architecture.

2. **Contribution 2 (Removed):** The secondary Machine Learning Predictor has been dropped to maintain intense focus solely on the DRL agent.

---

## D9: Hypothesis Sheet

- **H1:** An end-to-end DRL scheduling agent will match or exceed GREEN's carbon reduction (up to ~32%) but will reduce standard JCT inflation penalties by avoiding forced preemptions inherent to rigid queuing heuristics.
- **H2 (Removed):** Dropped along with the supervised ML efficiency predictor.
- **H3:** When subjected to varying Gaussian noise representing real-world grid anomalies (e.g., 15% forecast variance), the robust ML-driven scheduler will restrict carbon savings degradation to `≤ 5%`, drastically outperforming heuristic schedulers.

---

## D10: Scope Boundary Sheet

### In-Scope
- Developing, training, and tuning a Deep Reinforcement Learning (DRL) agent for dynamic cluster scheduling functionality (in `src/ml_engine/`).
- Constructing Python-based simulation environments (Gym) iterating Alibaba cluster traces for RL training.
- Generating pure theoretical baseline comparisons mimicking the results from GREEN.

### Out-of-Scope (Strictly Avoid)
- **Manual Heuristics / Symbolic Queues:** The project will NOT build manual MLFQ structures, threshold-based shifting equations, or hard-coded OS priority routines.
- **Custom OS/DS engine programming:** Disregard lower-level OS integrations or Data Structure (DS) trie/heap manipulations irrelevant to pure ML execution.
- **Modifying Job Internal Configs:** We will remain strictly model-agnostic (unlike Pollux). We will not adjust user Hyperparameters, Epoch bounds, or Gradient logic.
- **Physical Thermal/PUE optimization:** HVAC controls or CPU thermal scaling falls firmly outside the scope of the scheduling boundary.
