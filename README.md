# Parallel Shortest Path Algorithms using OpenMP

This repository contains **sequential and parallel implementations** of classical shortest path algorithms written in **C using OpenMP**. The project focuses on analyzing **scalability, speedup, and efficiency** on multicore systems by comparing parallel executions against sequential baselines across varying graph sizes and thread counts.

---

## 📚 Project Overview

This project was developed as part of the **Multicore Processors** course at **New York University**.
The goal is to study how different graph algorithms behave under shared-memory parallelism and to evaluate their performance characteristics on modern multicore architectures.

**Algorithms implemented:**
* **Bellman-Ford** (Sequential and Parallel)
* **Dijkstra** (Sequential and Parallel)
* **Floyd-Warshall** (Sequential and Parallel)

Graphs of varying sizes were generated and tested to measure execution time, speedup, and scalability.

---

## 🗂️ Repository Structure

```text
parallel-graph-algorithms/
│
├── src/
│   ├── bellman_ford_seq_parallel.c
│   ├── dijkstra_seq_parallel.c
│   ├── floyd_warshall_seq_parallel.c
│   └── generate_graph.c
│
├── scripts/
│   ├── run_bell_ford.sh
│   ├── run_dijk.sh
│   └── run_floyd.sh
│
├── report/
│   └── MulticoreFinal_ProjectReport.pdf
│
├── README.md
└── .gitignore

```

---

## 🛠️ Build Instructions

### Requirements

* **Compiler:** GCC with OpenMP support
* **OS:** Linux-based environment (Tested on CIMS Crunchy Servers)

### Compilation

Compile the algorithms using the following commands:

```bash
# Bellman-Ford
gcc -fopenmp src/bellman_ford_seq_parallel.c -o bellman_ford

# Dijkstra
gcc -fopenmp src/dijkstra_seq_parallel.c -o dijkstra

# Floyd-Warshall
gcc -fopenmp src/floyd_warshall_seq_parallel.c -o floyd

```

---

## 🚀 Run Instructions

Run the algorithms using the provided shell scripts. Each script automates execution across different graph sizes and thread counts.

```bash
./scripts/run_bell_ford.sh
./scripts/run_dijk.sh
./scripts/run_floyd.sh

```

---

## 📊 Experimental Setup

* **Platform:** NYU CIMS Crunchy Servers
* **Architecture:** Multicore shared-memory system
* **Threads tested:** 1, 2, 4, 8
* **Graph sizes:** Small to large synthetic graphs generated programmatically
* **Parallelization model:** OpenMP `parallel for`

---

## 📈 Performance Results Summary

### Bellman-Ford

* Exhibited the highest scalability among all algorithms.
* Significant speedup observed for large graphs.
* Performance improved notably up to 4 threads before showing diminishing returns.

### Floyd-Warshall

* Demonstrated consistent and predictable scaling.
* Benefited strongly from loop-level parallelism.
* Most suitable for dense graphs with higher computation intensity.

### Dijkstra

* Limited scalability due to inherent sequential priority queue operations.
* Parallel version showed marginal improvements only for large inputs.

---

## 📌 Key Insights

1. Algorithms with independent edge relaxations scale better under OpenMP.
2. Synchronization and shared data structures significantly limit parallel efficiency.
3. Thread counts beyond 4 often show diminishing returns due to overhead on this specific architecture.
4. Parallel graph algorithms benefit more from computation-heavy workloads.

---

## 📄 Report

A detailed explanation of implementation strategies, experimental methodology, performance graphs, and conclusions is available in the final report:

**[View Project Report (PDF)](/report/project_report.pdf)**

---

## 🔮 Future Work

* Implementation of optimized data structures for parallel Dijkstra.
* Exploration of hybrid shared and distributed memory models (MPI + OpenMP).
* Task-based OpenMP parallelism.
* Testing on real-world graph datasets.

---

## 🧠 Technologies Used

* **Language:** C
* **Library:** OpenMP
* **Compiler:** GCC
* **Scripting:** Linux Shell

