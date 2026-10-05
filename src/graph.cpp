#include "graph.h"
#include <iostream>
#include <queue>
#include <vector>

// Constructor to initialize sizing
VentilationGraph::VentilationGraph(int junctions_count) {
    num_junctions = junctions_count;
    junctions.resize(junctions_count);
    adj_list.resize(junctions_count);
}

// Add a node to our network
void VentilationGraph::addJunction(int id, const std::string& name) {
    if (id >= 0 && id < num_junctions) {
        junctions[id] = {id, name, 0.0};
    }
}

// Add a directed airway/tunnel edge
void VentilationGraph::addAirway(int id, int from, int to, double resistance, double initial_flow) {
    Airway new_airway = {id, from, to, resistance, initial_flow};
    airways.push_back(new_airway);
    
    // Store the index of this airway inside our adjacency list for the source junction
    int airway_index = airways.size() - 1;
    adj_list[from].push_back(airway_index);
}

// Simple display utility
void VentilationGraph::displayNetwork() const {
    std::cout << "\n=== Mine Ventilation Network Graph ===\n";
    for (const auto& airway : airways) {
        std::cout << "Airway " << airway.id << ": Junction " << airway.from_junction 
                  << " (" << junctions[airway.from_junction].name << ") -> Junction " 
                  << airway.to_junction << " (" << junctions[airway.to_junction].name << ") | "
                  << "R = " << airway.resistance << ", Q = " << airway.flow_rate << " m3/s\n";
    }
}

// Standard DSA Traversal: Find a valid airway route using BFS
void VentilationGraph::printPathsBFS(int start_junction, int end_junction) const {
    if (start_junction < 0 || start_junction >= num_junctions || 
        end_junction < 0 || end_junction >= num_junctions) {
        std::cout << "Invalid start or end junction IDs.\n";
        return;
    }

    std::queue<int> q;
    std::vector<bool> visited(num_junctions, false);
    // parent_airway[v] stores the index of the airway used to reach junction v
    std::vector<int> parent_airway(num_junctions, -1); 
    // parent_node[v] stores the junction we came from
    std::vector<int> parent_node(num_junctions, -1);

    // Initialise tracking
    q.push(start_junction);
    visited[start_junction] = true;

    bool found = false;

    // Standard BFS Loop
    while (!q.empty()) {
        int current = q.front();
        q.pop();

        if (current == end_junction) {
            found = true;
            break;
        }

        // Traverse all outgoing airways from the current junction
        for (int airway_idx : adj_list[current]) {
            const Airway& edge = airways[airway_idx];
            int next_junction = edge.to_junction;

            if (!visited[next_junction]) {
                visited[next_junction] = true;
                parent_airway[next_junction] = airway_idx;
                parent_node[next_junction] = current;
                q.push(next_junction);
            }
        }
    }

    // Reconstruction and printing of path route
    if (!found) {
        std::cout << "No airflow route found from " << junctions[start_junction].name 
                  << " to " << junctions[end_junction].name << "\n";
        return;
    }

    std::cout << "\nAirflow Path Route found via BFS:\n";
    int curr = end_junction;
    std::vector<int> path_edges;

    while (curr != start_junction) {
        path_edges.push_back(parent_airway[curr]);
        curr = parent_node[curr];
    }

    // Print out trace from start to finish
    std::cout << junctions[start_junction].name;
    for (int i = path_edges.size() - 1; i >= 0; --i) {
        const Airway& edge = airways[path_edges[i]];
        std::cout << " ==[Airway " << edge.id << " (Q=" << edge.flow_rate << " m3/s)]==> " 
                  << junctions[edge.to_junction].name;
    }
    std::cout << "\n";
}
