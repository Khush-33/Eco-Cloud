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