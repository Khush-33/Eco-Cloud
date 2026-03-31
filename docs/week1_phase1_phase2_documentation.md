# Eco-Cloud Week 1 Documentation
## Phase 1 and Phase 2 Research Planning (Based on Base Paper)

Author context: Eco-Cloud research track
Date: 2026-03-31
Base paper used: GREEN: Carbon-efficient Resource Scheduling for Machine Learning Clusters (NSDI 2025)
Source text used for analysis: base_research_paper_extracted.txt (17 pages extracted)

---

## 0. What this document gives you

This document gives a complete Week 1 execution plan for:
1. Phase 1: Decode the base paper completely.
2. Phase 2: Define your exact research gap and direction.

The goal is simple: by the end of Week 1, you should know exactly:
1. What GREEN solved.
2. What GREEN did not solve.
3. What your Eco-Cloud paper will claim as novelty.

---

## 1. Quick base-paper anchors (already extracted)

Use these line anchors from base_research_paper_extracted.txt while reading:
1. Abstract: line 26
2. Introduction: line 45
3. Background and motivations: line 162
4. GREEN overview: line 382
5. Carbon tracker: line 440
6. GREEN optimizers: line 497
7. MLFQ scheduling: line 748
8. Implementation: line 896
9. Evaluation: line 916
10. Limitations: line 1356
11. Conclusion: line 1413
12. Appendix (deep dive experiments): line 1706

Key reported claims in the base paper:
1. Up to 41.2% cluster carbon-footprint reduction.
2. 12% peak power reduction.
3. 3.6% to 5.9% average JCT tradeoff vs best JCT baseline.
4. 32.2% average carbon reduction across 4 regions.
5. 21% reduction even when no temporal shifting variant is used.
6. 23.9% average reduction vs Pollux on compatible jobs.
7. 31.6% reduction in year-long simulator vs Tiresias.

---

## 2. Week 1 structure at high level

Week 1 should be split in two parts:
1. Days 1 to 4: Phase 1 (paper decoding and evidence extraction).
2. Days 5 to 7: Phase 2 (gap definition and research-position locking).

Output at the end of Week 1:
1. Base-paper understanding pack.
2. Research gap brief for Eco-Cloud.
3. Initial hypothesis set and experiment-intent table.

---

## 3. Phase 1: Decode the base paper completely

### Phase 1 objective
Build a factual map of the base paper so you do not copy its contribution accidentally and do not miss its hidden weaknesses.

### Phase 1 deliverables
You must produce all 5 deliverables below.

#### D1: One-page paper summary (in your own words)
Must include:
1. Problem statement.
2. Why existing schedulers are insufficient.
3. Core method design (Carbon Tracker + two optimizers + MLFQ).
4. Main results.
5. Main limitations.

#### D2: Claim-evidence table
For every major claim, record:
1. Claim text.
2. Metric used.
3. Compared baseline.
4. Dataset/workload used.
5. Where proven (figure/table/section).
6. Threat to validity.

Template:

| Claim ID | Claim | Metric | Baseline | Evidence location | Confidence | Notes |
|---|---|---|---|---|---|---|
| C1 | Carbon reduction up to 41.2% | Cluster carbon footprint | Tiresias/EcoVisor/GAIA | Evaluation figures | High | Depends on carbon-intensity pattern |

#### D3: Method reconstruction sheet
Reconstruct the method as if you had to re-implement it.

Include:
1. Inputs required by GREEN.
2. Runtime data needed (power, progress, carbon intensity).
3. Optimizer equations and variables.
4. Queue behavior and preemption logic.
5. Hyperparameters and knobs (gamma, mu, upper-queue cap, quantum).

#### D4: Evaluation reconstruction sheet
Capture all experiment choices:
1. Cluster/testbed setup.
2. Baselines and how they were configured.
3. Workloads and trace design.
4. Metrics and definitions.
5. Ablation and sensitivity checks.

#### D5: Limitation and opportunity log
Extract explicit and implicit limitations.

Explicit (reported by authors):
1. Less gain in low-carbon or low-variance regions.
2. Potential starvation concerns in congested conditions.
3. Dependence on carbon-intensity forecasting quality.
4. Not ideal for latency-sensitive inference jobs.

Implicit (you should note):
1. Complexity of reproducibility due Slurm plugin integration.
2. Possible sensitivity to power measurement quality.
3. Scheduling overhead vs gain for small jobs.
4. Fairness behavior across long-tail workloads under repeated preemption.

---

### Step-by-step execution for Phase 1

#### Step 1: Build section map and paper skeleton
Action:
1. Create a document with each section as one heading.
2. Under each heading, write 3 to 5 bullets of key points.

Done when:
1. You can explain GREEN end-to-end in 5 minutes without opening the paper.

#### Step 2: Extract definitions and equations
Action:
1. List all formal terms: carbon footprint, energy efficiency, degradation factor, shifting factor, priority.
2. Rewrite each in your notation.
3. Note what can be measured directly and what is estimated.

Done when:
1. You can explain each equation variable without ambiguity.

#### Step 3: Reconstruct algorithmic flow
Action:
1. Draw data flow: telemetry -> factors -> optimizers -> scheduler.
2. Draw control flow: job arrival -> upper queue scaling -> lower queue shifting -> preemption.

Done when:
1. You can write pseudocode from memory at high level.

#### Step 4: Decode experiments rigorously
Action:
1. Build a baseline matrix and compare what each baseline optimizes.
2. Extract all reported tradeoffs (carbon vs JCT vs tail JCT).
3. Separate absolute results from ratio results.

Done when:
1. You can answer: "Where exactly does GREEN win, and where does it lose?"

#### Step 5: Build replication and risk notes
Action:
1. List what is directly reproducible for your environment.
2. List what is not directly reproducible and why.
3. Mark replacement strategy for each missing part.

Done when:
1. You have no hidden assumptions left.

---

### Phase 1 quality checklist

Mark each item Yes/No:
1. I can state the paper problem in one sentence.
2. I can state the novelty in one sentence.
3. I can explain every core equation variable.
4. I can describe the two-queue design without confusion.
5. I can justify every major reported number with evidence location.
6. I have listed both explicit and implicit limitations.
7. I have identified at least 5 opportunities for extension.

If any answer is No, Phase 1 is not complete.

---

## 4. Phase 2: Define your research gap and direction

### Phase 2 objective
Convert your understanding from Phase 1 into a clear, defensible, and testable research position for Eco-Cloud.

### Phase 2 deliverables

#### D6: Gap statement set
Write 3 candidate gap statements, then choose 1 final gap.

Good format:
"While GREEN does X under Y assumptions, it does not handle Z under W constraints, which matters because ..."

#### D7: Research question set
Create:
1. One primary research question (RQ1).
2. Two supporting questions (RQ2, RQ3).

Example structure:
1. RQ1: Can a hybrid scheduler reduce carbon footprint with lower JCT penalty than GREEN-like strategies under constrained academic clusters?
2. RQ2: Which component contributes most: forecasting, rule engine, or optimization scheduler?
3. RQ3: How robust is the method under forecast error and failure bursts?

#### D8: Contribution lock
Freeze exactly 2 to 3 contributions.

For each contribution define:
1. Novel artifact (algorithm, model, policy, framework).
2. Expected measurable gain.
3. Required experiment to prove it.

#### D9: Hypothesis sheet
Define at least 3 hypotheses in measurable form.

Template:
1. H1: Proposed method reduces total carbon by >= X% vs baseline B under scenario S.
2. H2: JCT increase remains <= Y% vs baseline B in scenario S.
3. H3: Under forecast noise N, carbon gain degrades by <= Z%.

#### D10: Scope boundary sheet
List in-scope and out-of-scope items to avoid project drift.

---

### Step-by-step execution for Phase 2

#### Step 1: Build opportunity bank
Action:
1. From D5 (limitations), create 10 candidate opportunities.
2. Group them into themes: algorithmic, systems, robustness, evaluation.

Done when:
1. Each opportunity is written as "change + expected impact".

#### Step 2: Score opportunities
Use this scoring model (1 to 5 each):
1. Novelty potential.
2. Feasibility in your environment.
3. Experimental testability.
4. Publication value.

Score formula:
Total score = Novelty + Feasibility + Testability + Publication value

Done when:
1. Top 3 opportunities are selected by score.

#### Step 3: Write candidate gap statements
Action:
1. Write 3 precise gap statements.
2. Validate each: is it truly not solved in the base paper?

Done when:
1. One gap is selected and approved as primary gap.

#### Step 4: Define contribution package
Action:
1. Select 2 to 3 contributions aligned to the chosen gap.
2. Map each contribution to one experiment block.

Done when:
1. Every contribution has at least one direct proof experiment.

#### Step 5: Freeze hypotheses and acceptance criteria
Action:
1. Define thresholds for carbon gain and JCT tradeoff.
2. Define robustness acceptance (noise, failures, load spikes).

Done when:
1. You can reject or accept your method objectively from data.

---

### Phase 2 quality checklist

Mark each Yes/No:
1. My final gap statement is specific and testable.
2. My RQ1, RQ2, RQ3 are aligned and non-overlapping.
3. My contributions are limited to 2 to 3 items only.
4. Every contribution has a proof plan.
5. My hypotheses contain numeric targets.
6. I have written in-scope vs out-of-scope boundaries.
7. I have a clear baseline comparison plan.

If any answer is No, Phase 2 is not complete.

---

## 5. Week 1 day-wise execution plan

### Day 1
1. Read abstract, intro, motivation, related categories.
2. Create D1 skeleton and section map.

### Day 2
1. Decode equations and factors.
2. Create D3 method reconstruction sheet.

### Day 3
1. Decode implementation and evaluation setup.
2. Create D4 evaluation reconstruction sheet.

### Day 4
1. Build D2 claim-evidence table.
2. Build D5 limitation and opportunity log.

### Day 5
1. Build opportunity bank and score matrix.
2. Generate 3 candidate gap statements.

### Day 6
1. Select final gap.
2. Draft RQs and contribution lock (D7, D8).

### Day 7
1. Finalize hypothesis sheet and scope boundary (D9, D10).
2. Write Week 1 summary memo (1 page).

---

## 6. What success looks like at end of Week 1

You are ready for Week 2 only if all are true:
1. You can explain GREEN better than a normal reader.
2. You have one selected research gap for Eco-Cloud.
3. You have 2 to 3 frozen contributions.
4. You have numeric hypotheses and acceptance criteria.
5. You have a small but precise scope for implementation.

If these are complete, Week 2 should start with method design and experiment matrix design.

---

## 7. Suggested file outputs to create next

Create these files inside docs/ as you execute:
1. docs/week1_d1_one_page_summary.md
2. docs/week1_d2_claim_evidence_table.md
3. docs/week1_d3_method_reconstruction.md
4. docs/week1_d4_evaluation_reconstruction.md
5. docs/week1_d5_limitations_opportunities.md
6. docs/week1_d6_to_d10_gap_rq_hypothesis.md

This keeps your paper journey traceable and reviewer-ready.

---

## 8. Notes specific to Eco-Cloud

For Eco-Cloud, likely strong directions (to test in Phase 2 scoring):
1. Hybrid symbolic + predictive + optimization scheduling under constrained capacity.
2. Robust scheduling under forecast uncertainty and sudden failure bursts.
3. Fairness-aware carbon optimization for mixed-priority workloads.
4. Lightweight deployment path for academic clusters with minimal scheduler intrusion.

Do not lock these now without scoring. Use the Phase 2 scoring matrix first.
