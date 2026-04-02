# Phase 3: Method Specification and Experiment Contract

## D11: Problem Formalization
**Optimization Target:**
Develop an end-to-end Deep Reinforcement Learning (DRL) scheduling agent to optimize multi-tenant ML cluster operations under volatile carbon intensity. No supplementary supervised predictors will be trained, allowing explicit focus on the RL logic.

**Reporting Objective (Reward Function):**
Instead of a purely aggressive carbon cut, we balance the objectives:
J = (0.5 * Carbon_Footprint_Penalty) + (0.4 * JCT_Delay_Penalty) + (0.1 * Preemption_Overhead_Penalty)

## D12: DRL Environment Specification
**State Space:**
- Time index and current normalized Carbon Intensity (with simulated forecast noise).
- Queue summary features (job count, age distribution, power bands).
- Cluster capacity and current GPU utilization.

**Action Space (Discrete - KISS Principle):**
- **Priority Sorting:** The DRL agent acts as a sorter. It outputs a discrete ranking of the jobs currently in the queue (e.g., Job 3 > Job 1 > Job 2).
- *Why:* This offloads the complex capacity-packing algorithm to a standard allocation loop, avoiding illegal action states (like the ML trying to assign 100 GPUs when only 32 exist).

## D13: Reward Function Specification
- **Carbon Penalty (50%):** Tied directly to the integrated carbon emissions (kg CO2) accumulated over the step, punishing execution during peak dirty-grid hours.
- **JCT Penalty (40%):** Delay metrics for active and queued jobs. This high weight prevents the agent from infinitely pausing jobs.
- **Preemption Overhead Penalty (10%):** A flat subtraction applied whenever a job's state flips from running to paused, teaching the agent not to aggressively thrash the queue.

## D14: (Removed to reduce Scope)
**Removed:** The secondary "Predictor Model" has been completely removed from the project scope to adhere to the KISS principle. The architecture will rely 100% on the internal policies learned organically by the DRL scheduling agent.

## D15: Experiment Matrix and Acceptance Criteria
**Dimensions to Test:**
- **Workload Congestion:** Light, Moderate, High.
- **Forecast Noise (Gaussian Variance):** 0%, 5%, and 15% (no complex API-failure setups to prevent over-engineering).
- **Baselines to Run:** Tiresias (JCT-focused baseline) and GREEN (Heuristic carbon-focused baseline).

**Acceptance Criteria:**
1. **Carbon Target:** At least 20% footprint reduction compared to Tiresias on high-variance grids.
2. **JCT Target:** No more than a 6% average JCT increase compared to Tiresias. 
3. **Robustness Target:** Under 15% forecast-noise, the carbon savings drop by no more than 5% compared to the 0% noise condition (proving the DRL handles noise better than rigid rules).
