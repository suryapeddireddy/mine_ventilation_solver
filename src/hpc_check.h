#ifndef HPC_CHECK_H
#define HPC_CHECK_H

#include "graph.h"

class HPCCheck {
public:
    // Performs a multi-threaded parallel verification of network flow continuity
    static bool verifyContinuityParallel(const VentilationGraph& graph, int num_threads);
};

#endif // HPC_CHECK_H
