#include <iostream>
#include <vector>
#include "graph.h"
#include "solver.h"
#include "hpc_check.h"


int main() {
    std::cout << "=== Starting Mine Ventilation Solver Testbench ===\n" << std::endl;

    // 1. Initialize using your exact class name
    VentilationGraph network;
    
    // Add nodes/airways using the actual methods defined in your graph.h
    network.addNode(0); 
    network.addNode(1); 
    network.addNode(2); 
    network.addNode(3); 

    // Assuming parameters match your signature (source, target, resistance, initial_flow)
    network.addAirway(0, 1, 0.05, 50.0);  
    network.addAirway(1, 2, 0.25, 25.0);  
    network.addAirway(1, 2, 0.30, 25.0);  
    network.addAirway(2, 3, 0.08, 50.0);  

    std::cout << "[✓] VentilationGraph initialization successful." << std::endl;

    // 2. Call the solver using your exact function name
    std::cout << "Running Hardy Cross simulation iterators..." << std::endl;
    VentilationSolver::computePressuresAndImbalances(network);
    std::cout << "[✓] Solver routine finished execution." << std::endl;

    // 3. Trigger your multi-threaded HPC check layer
    std::cout << "Spawning multi-threaded validation layer..." << std::endl;
    
    // NOTE: If your verification method name inside hpc_check.h differs from 
    // 'verifyConservation', change it here to match your exact signature!
    bool isFlowValid = HPCCheck::runVerification(network); 
    
    if (isFlowValid) {
        std::cout << "[✓] HPC Verification Passed: Flows verified via parallel checks!" << std::endl;
    } else {
        std::cerr << "[X] HPC Verification Failed: Discrepancy found." << std::endl;
    }

    std::cout << "\n=== Testbench Execution Completed ===" << std::endl;
    return 0;
}
