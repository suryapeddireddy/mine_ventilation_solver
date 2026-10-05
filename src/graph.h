#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

// Represents a node in the mine network (e.g., shaft bottom, production face, fan station)
struct Junction {
    int id;
    std::string name;
    double net_flow; // Total flow entering minus leaving (should be 0 for continuity)
};

// Represents a directed edge/tunnel between two junctions
struct Airway {
    int id;
    int from_junction;
    int to_junction;
    double resistance; // Aerodynamic resistance (R)
    double flow_rate;  // Airflow rate in m³/s (Q)
};

// Main Graph class using an intermediate Adjacency List structure
class VentilationGraph {
private:
    int num_junctions;
    std::vector<Junction> junctions;
    std::vector<Airway> airways;
    
    // Adjacency list: map junction ID to a list of Airway indices originating from it
    std::vector<std::vector<int>> adj_list;

public:
    // Constructor
    VentilationGraph(int junctions_count);
    // Add this inside the public section of VentilationGraph in graph.h
void updateAirwayFlow(int id, double new_flow) {
    if(id >= 0 && id < (int)airways.size()) {
        airways[id].flow_rate = new_flow;
    }
}


    // Core Setup Methods
    void addJunction(int id, const std::string& name);
    void addAirway(int id, int from, int to, double resistance, double initial_flow);

    // Standard DSA Traversal & Lookups
    void displayNetwork() const;
    void printPathsBFS(int start_junction, int end_junction) const;
    
    // Getters for solver and HPC verification
    const std::vector<Airway>& getAirways() const { return airways; }
    const std::vector<Junction>& getJunctions() const { return junctions; }
    int getJunctionCount() const { return num_junctions; }
};

#endif // GRAPH_H
