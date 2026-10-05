#include "hpc_check.h"
#include <iostream>
#include <vector>
#include <thread>
#include <cmath>

// Worker function that each thread will execute independently
void verifyJunctionRange(int thread_id, int start_idx, int end_idx, 
                         const std::vector<Junction>& junctions, 
                         const std::vector<Airway>& airways, 
                         std::vector<bool>& thread_results) {
    
    bool chunk_valid = true;
    double tolerance = 1e-4; // Small margin for floating-point calculations

    // Loop through the assigned chunk of junctions
    for (int i = start_idx; i < end_idx; ++i) {
        double current_junction_flow = 0.0;
        int j_id = junctions[i].id;

        // Calculate flow balance for this junction by checking all airways
        for (const auto& airway : airways) {
            if (airway.from_junction == j_id) {
                current_junction_flow -= airway.flow_rate; // Flow leaving
            }
            if (airway.to_junction == j_id) {
                current_junction_flow += airway.flow_rate; // Flow entering
            }
        }

        // Verify if the balance is close to zero (Mass Continuity)
        if (std::abs(current_junction_flow) > tolerance) {
            chunk_valid = false;
            // We don't print inside the thread loop to avoid slowing it down (HPC best practice)
        }
    }

    // Store the final outcome for this thread's chunk
    thread_results[thread_id] = chunk_valid;
}

bool HPCCheck::verifyContinuityParallel(const VentilationGraph& graph, int num_threads) {
    const auto& junctions = graph.getJunctions();
    const auto& airways = graph.getAirways();
    int total_junctions = graph.getJunctionCount();

    if (total_junctions == 0) return true;
    if (num_threads > total_junctions) num_threads = total_junctions;

    std::cout << "\n[HPC Layer] Starting multi-threaded verification using " 
              << num_threads << " workers...\n";

    std::vector<std::thread> workers;
    std::vector<bool> thread_results(num_threads, true);

    // Load Balancing: Divide junctions evenly across our worker threads
    int chunk_size = total_junctions / num_threads;
    int remainder = total_junctions % num_threads;

    int current_start = 0;
    for (int i = 0; i < num_threads; ++i) {
        int current_end = current_start + chunk_size;
        if (i == num_threads - 1) {
            current_end += remainder; // Last thread handles any leftover nodes
        }

        // Launch the thread worker
        workers.push_back(std::thread(verifyJunctionRange, i, current_start, 
                                      current_end, std::ref(junctions), 
                                      std::ref(airways), std::ref(thread_results)));
        
        current_start = current_end;
    }

    // Synchronisation barrier: Wait for all threads to complete execution
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    // Reduction Step: Aggregate results from all threads
    bool entire_network_valid = true;
    for (bool res : thread_results) {
        if (!res) {
            entire_network_valid = false;
            break;
        }
    }

    if (entire_network_valid) {
        std::cout << "[HPC Layer] SUCCESS: All junctions satisfy flow continuity.\n";
    } else {
        std::cout << "[HPC Layer] WARNING: Continuity violation detected in network values!\n";
    }

    return entire_network_valid;
}
