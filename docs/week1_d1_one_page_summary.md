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