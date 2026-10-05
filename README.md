# High-Performance Mine Ventilation Network Simulator & Validator

A high-performance C++ simulation and verification engine designed to model aerodynamic network graph configurations for underground mines. The system extracts fundamental ventilation circuits utilizing advanced graph algorithms and deploys a multi-threaded asynchronous layer to validate mass flow rate and junction continuity balances across the network.

## 🚀 Key Achievements & Results
* **Custom Object-Oriented Graph Infrastructure:** Developed a zero-overhead adjacency-list graph structure from scratch (`VentilationGraph`), removing dependency bottlenecks common in massive black-box libraries like Boost.Graph.
* **Deterministic Circuit Extraction:** Implemented a Union-Find Disjoint Set forest alongside Breadth-First Search (BFS) traversals to reliably compute independent atmospheric closed loops (m - n + 1) and cross-sectional flow path orientations.
* **Asynchronous Multi-Threaded Validation:** Achieved a high-efficiency verification pipeline that utilizes static load-balancing to partition graph segments across independent CPU worker threads.
* **Optimized Multi-Core Reduction:** Engineered a synchronized concurrent reduction step to validate network-wide mass balance conservation rules without thread thrashing, cache contention, or console-blocking bottlenecks.

---

## 🛠️ Architecture & Core Components

The codebase utilizes a strictly decoupled, modular design ensuring clear separation between data modeling, mathematical simulation, and high-performance verification:

* **`graph.h` / `graph.cpp` (`VentilationGraph`):** The primary data structure layer. It models underground environments as spatial configurations using explicit `Junction` nodes and directed `Airway` edge parameters (Aerodynamic Resistance R, Volumetric Flow Rate Q).
* **`solver.h` / `solver.cpp` (`VentilationSolver`):** The core physical computation routine. It tracks directional air vectors to calculate instant aerodynamic pressure drops based on square-law fluid dynamics (P = R ⋅ Q ⋅ |Q|).
* **`hpc_check.h` / `hpc_check.cpp` (`HPCCheck`):** The performance validation engine. It partitions network nodes into distinct memory chunks, launching concurrent thread tasks to check junction flow safety bounds against Kirchhoff's Current Law.
* **`main.cpp`:** The orchestration script. Handles raw network configuration stream file parsing, structural loop space tracking, asset initialization, and engine execution.

---

## 🔬 Mathematical Basis & Fluid Dynamics

The solver applies classic mining engineering mechanics combined with numerical constraints:

1. **Airway Pressure Drop (Atkinson's Relation):**
   \[P = R \cdot Q^2 \cdot \text{sgn}(Q)\]
   Where P is pressure loss in Pascals (Pa), R is the aerodynamic resistance factor (N⋅s²/m⁸), and Q is volumetric flow rate (m³/s).

2. **Junction Conservation Continuity (Kirchhoff's Current Law):**
   \[\sum Q_{\text{in}} - \sum Q_{\text{out}} = 0\]
   For any stable network branch configuration, the residual fluid imbalance mass must converge within a predefined safety margin (ε < 10⁻⁴).

---

## 💻 Sample Project Execution Trace

When executed with a 3-node, 4-airway structural model tracking high initial baseline flows, the simulator cleanly logs physical pressure distribution profiles and handles automated multithreaded constraint failures:

```text
3 nodes, 4 airways loaded.

=== Launching Numerical Solver Pipeline ===

=== Calculating Airway Pressure Drops (P = R * Q^2) ===
Airway 0 (Junction_0 -> Junction_1): Pressure Drop = 1 Pa
Airway 1 (Junction_1 -> Junction_2): Pressure Drop = 10 Pa
Airway 2 (Junction_1 -> Junction_2): Pressure Drop = 40 Pa
Airway 3 (Junction_2 -> Junction_0): Pressure Drop = 2 Pa

=== Junction Flow Imbalances calculated ===
Junction 0 (Junction_0): Net Imbalance = 0 m3/s
Junction 1 (Junction_1): Net Imbalance = -10 m3/s
Junction 2 (Junction_2): Net Imbalance = 10 m3/s

=== Running Parallel HPC Diagnostics ===

[HPC Layer] Starting multi-threaded verification using 3 workers...
[HPC Layer] WARNING: Continuity violation detected in network values!
```

---

## 🛠️ Build & Compilation Guide

### Pre-requisites
* A modern C++ compiler supporting standard language features (**C++17** or higher recommended).
* POSIX thread capabilities matching the `-pthread` library framework flags.

### Compilation String
Run the unified linking command from your terminal interface to build the compiled production binary output:

```bash
g++ -std=c++17 -pthread src/graph.cpp src/solver.cpp src/hpc_check.cpp src/main.cpp -o vent
```

### Running the Simulator
To execute the runtime image, pass it the structural file location path inside your directory structure:

```bash
./vent data/test1.txt
```
