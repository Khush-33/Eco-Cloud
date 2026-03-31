# Eco-Cloud Next Phases Guide
## After completing Phase 1 and Phase 2

Date: 2026-03-31
Project: Eco-Cloud
Inputs reviewed:
- docs/week1_d1_one_page_summary.md
- docs/week1_d2_claim_evidence_table.md
- docs/week1_d3_method_reconstruction.md
- docs/week1_d4_evaluation_reconstruction.md
- docs/week1_d5_limitations_opportunities.md
- docs/week1_d6_to_d10_gap_rq_hypothesis.md

---

## 1) Where you are now

You have completed the hardest early work correctly:
1. You understand the base paper deeply.
2. You extracted claims and limitations.
3. You selected a strong primary direction: ML-first scheduler with robustness to forecast noise.

Now the focus must shift from analysis to proof.

Your next phases should be:
1. Phase 3: Method specification and experiment contract.
2. Phase 4: Infrastructure and baseline implementation.
3. Phase 5: Model development and training.
4. Phase 6: Full evaluation and ablations.
5. Phase 7: Paper writing and artifact packaging.

---

## 2) Critical correction before Phase 3 starts

Your current hypothesis set is good, but one item must be tightened now:
1. H1 currently says "match or exceed" without explicit numeric target.

Replace with measurable thresholds so your method can be accepted or rejected objectively.

Recommended acceptance thresholds:
1. Carbon reduction target: at least 20% versus Tiresias on primary scenarios.
2. JCT penalty target: no more than 6% average JCT increase versus Tiresias.
3. Robustness target: under 15% forecast-noise injection, carbon gain drop no more than 7% from no-noise condition.
4. Stability target: scheduling decision variance under fixed seed below predefined bound.

---

## 3) Phase 3 (Week 2): Method Specification

### Objective
Lock the exact method and the exact experiment contract before heavy coding.

### Deliverables
1. D11: Problem formalization sheet.
2. D12: DRL environment specification.
3. D13: Reward function specification.
4. D14: Predictor model specification.
5. D15: Experiment matrix and acceptance criteria.

### Step-by-step

#### Step 3.1: Formalize optimization target
Define one scalar training objective and one reporting objective.

Example reporting objective:
J = w1*Carbon + w2*AvgJCT + w3*TailJCT + w4*SLA

Do not optimize all metrics directly at once without weights. Fix weight ranges now.

#### Step 3.2: Define DRL MDP contract
State should include:
1. Time index and forecast/noise confidence.
2. Queue summary features (job count, age distribution, power bands).
3. Cluster capacity and current utilization.
4. Recent preemption cost history.

Action should include:
1. Allocate capacity slice by job class or job IDs.
2. Optional defer/advance decision with bounded window.

Reward should include:
1. Carbon penalty.
2. JCT penalty.
3. Preemption overhead penalty.
4. Fairness penalty.

#### Step 3.3: Define supervised predictor contract
Input features:
1. Job metadata (type, size proxies, initial throughput).
2. Early run telemetry window.
3. Resource allocation context.

Outputs:
1. Predicted energy-efficiency degradation.
2. Predicted short-horizon power draw.

Target quality:
1. MAE and R2 thresholds (not only accuracy).

#### Step 3.4: Freeze experiment matrix
Include dimensions:
1. Workload regime: light, moderate, high congestion.
2. Carbon pattern: high variance, low variance.
3. Forecast noise: 0%, 5%, 10%, 15%.
4. Failure regime: none, random, burst.

For each cell, define:
1. Baselines to run.
2. Number of seeds (minimum 10, ideally 20 to 30).
3. Metrics to report.

### Phase 3 Definition of Done
1. You can hand the spec to another person and they can implement without ambiguity.
2. Acceptance criteria are numeric.
3. All experiments required for claims are listed before implementation.

---

## 4) Phase 4 (Week 3): Infrastructure and Baselines

### Objective
Build trusted experiment infrastructure before training advanced models.

### Deliverables
1. D16: Reproducible simulator runner.
2. D17: Baseline implementations and validation report.
3. D18: Logging schema and dataset export pipeline.

### Implementation guidance for Eco-Cloud structure
Use these target locations:
1. src/integration/: scenario generators, forecast-noise injection, run configurations.
2. src/ml_engine/: predictor training and DRL training code.
3. tests/: metric and reward sanity tests.

### Required baseline set
1. FCFS.
2. Priority.
3. GREEN-like heuristic baseline (simplified if needed, but explicit).
4. Optional GA baseline from your parallel project for comparison.

### Non-negotiable engineering checks
1. Fixed random seeds and deterministic replay option.
2. Per-step logs for action, state summary, reward components.
3. Unified metrics output format (json/csv).
4. Experiment config file per run.

### Phase 4 Definition of Done
1. All baselines run end-to-end on at least one scenario.
2. Metrics are consistent across reruns under same seed.
3. Output artifacts are machine-parseable.

---

## 5) Phase 5 (Weeks 4-5): Model Development

### Objective
Train and validate predictor and DRL scheduler with controlled comparisons.

### Deliverables
1. D19: Predictor model benchmark report.
2. D20: DRL training report with learning curves.
3. D21: Policy checkpoint registry and selection criteria.

### Recommended sequence
1. Predictor first:
- Try linear/regression tree baseline first.
- Then try gradient boosting or small neural model.
- Select by MAE + inference latency + calibration.

2. DRL second:
- Start with PPO as baseline DRL algorithm.
- Add curriculum: low-load scenarios first, then high-load.
- Keep reward decomposition visible in logs.

3. Integration:
- Compare DRL with and without predictor features.
- Validate that predictor actually improves decisions.

### Phase 5 Definition of Done
1. DRL beats at least one heuristic baseline on primary metric.
2. Predictor improves either carbon or JCT tradeoff measurably.
3. Training is stable across multiple seeds.

---

## 6) Phase 6 (Weeks 6-7): Full Evaluation and Ablations

### Objective
Generate publication-quality evidence with significance and robustness.

### Deliverables
1. D22: Main results table pack.
2. D23: Ablation and sensitivity report.
3. D24: Statistical significance report.

### Required analyses
1. Main comparison across all baselines.
2. Ablation A: DRL without predictor.
3. Ablation B: predictor without DRL.
4. Ablation C: reward term removals.
5. Sensitivity: reward weights, noise levels, congestion levels.

### Statistics requirements
1. Mean plus standard deviation.
2. 95% confidence intervals.
3. Paired significance test across seeds.

### Phase 6 Definition of Done
1. Every contribution has direct empirical evidence.
2. Negative results are documented, not hidden.
3. Figures are reproducible from scripts.

---

## 7) Phase 7 (Weeks 8-9): Paper Writing and Packaging

### Objective
Convert results into an IEEE-ready manuscript and reproducible artifact.

### Writing order
1. Results and figures first.
2. Method and setup second.
3. Introduction and related work third.
4. Abstract and title last.

### Deliverables
1. D25: Paper draft v1.
2. D26: Internal review checklist report.
3. D27: Final camera-ready style draft.

### Paper claim discipline
For each claim in paper:
1. Point to one figure/table.
2. State scenario and baseline.
3. Include tradeoff, not only gain.

---

## 8) Immediate next 10 days plan

Day 1:
1. Tighten hypotheses to numeric acceptance thresholds.
2. Freeze reward components and metric schema.

Day 2:
1. Write DRL environment spec (state/action/reward/termination).
2. Write predictor feature schema.

Day 3:
1. Build scenario configuration files.
2. Add forecast-noise injector and failure injector.

Day 4:
1. Implement FCFS and Priority baselines in shared runner.
2. Add unified metrics logger.

Day 5:
1. Implement GREEN-like heuristic baseline for comparison.
2. Validate baseline behavior with small scenarios.

Day 6:
1. Build predictor training pipeline and baseline model.
2. Export model metrics report.

Day 7:
1. Build initial DRL environment and random-policy sanity test.
2. Verify reward signal is non-degenerate.

Day 8:
1. Train first PPO model on simplified scenario.
2. Compare against FCFS and Priority quickly.

Day 9:
1. Run robustness mini-batch with 0% and 15% forecast noise.
2. Log gains and regressions.

Day 10:
1. Review results against acceptance criteria.
2. Decide tuning priorities for Week 3 onward.

---

## 9) Main risks and controls

Risk 1: DRL overfits to one scenario.
Control: train on mixed scenario curriculum, evaluate on held-out patterns.

Risk 2: Reward hacking.
Control: log per-term reward contributions and enforce penalty caps.

Risk 3: Baseline unfairness.
Control: keep same workload, same seeds, same metrics pipeline for all methods.

Risk 4: Too broad scope.
Control: keep exactly two locked contributions unless strong evidence demands extension.

---

## 10) Suggested new docs to create now

1. docs/week2_d11_problem_formalization.md
2. docs/week2_d12_drl_environment_spec.md
3. docs/week2_d13_reward_spec.md
4. docs/week2_d14_predictor_spec.md
5. docs/week2_d15_experiment_matrix.md

(DO NOT CREATE MULTIPLE DOCS, CREATE A ONE SINGLE DOC THAT HAS THE CONTETS OF ALL THESE DOCS)

If these five are complete, your Phase 3 is complete.
