#include "solver.h"
#include <cmath>
#include <iostream>
#include <vector>

void VentilationSolver::computePressuresAndImbalances(VentilationGraph& graph) {
    // We need to modify the flow imbalances, so we extract a copy or work via direct references.
    // To keep it simple, we directly compute the metrics using the getters.
    
    // 1. Reset all net flows at the junctions to 0
    // Because getters return const references, let's process the math transparently.
    std::vector<double> junction_net_flows(graph.getJunctionCount(), 0.0);
    const auto& airways = graph.getAirways();
    const auto& junctions = graph.getJunctions();

    std::cout << "\n=== Calculating Airway Pressure Drops (P = R * Q^2) ===\n";
    
    for (const auto& airway : airways) {
        // Calculate pressure drop using the basic mining ventilation physics formula
        double sign = (airway.flow_rate >= 0) ? 1.0 : -1.0;
        double pressure_drop = airway.resistance * airway.flow_rate * airway.flow_rate * sign;
        
        std::cout << "Airway " << airway.id << " (" << junctions[airway.from_junction].name 
                  << " -> " << junctions[airway.to_junction].name 
                  << "): Pressure Drop = " << pressure_drop << " Pa\n";

        // 2. Accumulate flow balance for continuity checks (Kirchhoff's Current Law / Mass Balance)
        // Flow leaving a junction is negative, flow entering is positive
        junction_net_flows[airway.from_junction] -= airway.flow_rate;
        junction_net_flows[airway.to_junction] += airway.flow_rate;
    }

    std::cout << "\n=== Junction Flow Imbalances calculated ===\n";
    for (int i = 0; i < graph.getJunctionCount(); ++i) {
        std::cout << "Junction " << i << " (" << junctions[i].name 
                  << "): Net Imbalance = " << junction_net_flows[i] << " m3/s\n";
    }
}
