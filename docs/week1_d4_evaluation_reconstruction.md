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