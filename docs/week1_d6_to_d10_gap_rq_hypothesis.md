# Phase 2: Gap, Questions, and Hypotheses (D6 - D10)

## D6: Gap Statement Set

### Candidate Gaps
1. **Candidate 1 (Forecasting Resilience):** While GREEN optimizes temporal shifting under the assumption of perfect real-time carbon intensity (CIC) forecasts, it does not securely handle real-world forecast noise or abrupt anomalies, which matters because inaccurate predictions can invert preemption priorities and actually accidentally increase structural grid emissions.
2. **Candidate 2 (Fairness & Priority):** While GREEN calculates cluster priority uniformly as a factor of raw node consumption and pure scaling degradation, it fails to encode administrative hierarchy or organizational fair-share bounds, which causes heavily parameterized research groups to incidentally throttle lean groups entirely out of prime operational windows.
3. **Candidate 3 (Latency vs. Flexibility Hybridization):** While GREEN explicitly disregards delay-sensitive (inference/serving) operations due to its hard MLFQ halt/shift mechanics, it fails to schedule environments that intrinsically mix micro-batch inference with long-tail batch training under fixed capacities.

### Final Selected Primary Gap (for Eco-Cloud Project)
**"While GREEN relies on rigid, formulaic heuristics (like MLFQ prioritization and piecewise shift functions) assuming perfect grid intensity knowledge, it degrades unpredictably under real-world Carbon Forecast noise and static thresholds. Our Eco-Cloud architecture abandons obsolete heuristic rule-engines entirely, instead applying an end-to-end Machine Learning approach (e.g., Deep Reinforcement Learning and predictive models) to inherently learn carbon-aware scheduling and job efficiency policies."** 
*(Selected because the project goal is to be a pure ML-driven solution, fully leveraging the `ml_engine` to solve the routing and efficiency problem data-scientifically rather than hard-coding queue rules).*

---

## D7: Research Question Set

- **RQ1 (Primary):** Can a Deep Reinforcement Learning (DRL) agent completely replace static scheduling heuristics to dynamically optimize the trade-off between cluster carbon footprint and Job Completion Time (JCT)?
- **RQ2 (Secondary/Systems):** Can we utilize a supervised ML model to accurately predict a job's energy efficiency degradation without forcing the cluster to manually profile jobs by continually shifting GPUs?
- **RQ3 (Secondary/Algorithmic):** How robust is a fully ML-driven scheduling pipeline against inherent noise in carbon grid forecasts compared to strictly deterministic, equation-based schedulers like GREEN?

---

## D8: Contribution Lock

We commit strictly to the following 2 ML-focused contributions to guarantee project focus:

1. **Contribution 1: DRL-Based Carbon-Aware Scheduler (Algorithm):** 
   - A Deep Reinforcement Learning scheduling agent trained to ingest state configurations (cluster capacity, real-time noisy CIC, job queues) and output allocation actions, fully eliminating the need for hard-coded MLFQ rules or priority formulas.
   - *Expected Gain:* Superior adaptability to grid noise and complex job profiles without relying on manually tuned thresholds (like the arbitrary `µ=2` parameter in GREEN).
   - *Proof:* Simulation of the Alibaba trace using an OpenAI Gym-style environment evaluated against the deterministic GREEN architecture.

2. **Contribution 2: ML-Driven Efficiency Predictor (Framework/Systems):**
   - An integrated supervised machine learning model that predicts a job's power and energy-efficiency scalability dynamically based on initial job metrics and metadata.
   - *Expected Gain:* Immediate, accurate resource scaling avoiding JCT taxes levied by excessive pausing and iterative hardware profiling.
   - *Proof:* Testbed trial running a scaled down 24-hr trace demonstrating fewer preemptions and faster resource allocations utilizing the ML predictor versus GREEN's feedback loops.

---

## D9: Hypothesis Sheet

- **H1:** An end-to-end DRL scheduling agent will match or exceed GREEN's carbon reduction (up to ~32%) but will reduce standard JCT inflation penalties by avoiding forced preemptions inherent to rigid queuing heuristics.
- **H2:** Integrating the supervised ML predictability model will forecast job energy scaling degradation with ≥ 85% accuracy off initial metadata, eliminating the requirement for manual cluster profiling steps entirely.
- **H3:** When subjected to varying Gaussian noise representing real-world grid anomalies (e.g., 15% forecast variance), the robust ML-driven scheduler will restrict carbon savings degradation to `≤ 5%`, drastically outperforming heuristic schedulers.

---

## D10: Scope Boundary Sheet

### In-Scope
- Developing, training, and tuning a Deep Reinforcement Learning (DRL) agent for dynamic cluster scheduling functionality (in `src/ml_engine/`).
- Constructing Python-based simulation environments (Gym) iterating Alibaba cluster traces for RL training.
- Developing and training the supervised ML model to predict job energy scalability.
- Generating pure theoretical baseline comparisons mimicking the results from GREEN.

### Out-of-Scope (Strictly Avoid)
- **Manual Heuristics / Symbolic Queues:** The project will NOT build manual MLFQ structures, threshold-based shifting equations, or hard-coded OS priority routines.
- **Custom OS/DS engine programming:** Disregard lower-level OS integrations or Data Structure (DS) trie/heap manipulations irrelevant to pure ML execution.
- **Modifying Job Internal Configs:** We will remain strictly model-agnostic (unlike Pollux). We will not adjust user Hyperparameters, Epoch bounds, or Gradient logic.
- **Physical Thermal/PUE optimization:** HVAC controls or CPU thermal scaling falls firmly outside the scope of the scheduling boundary.