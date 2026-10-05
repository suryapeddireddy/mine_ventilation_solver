#ifndef SOLVER_H
#define SOLVER_H

#include "graph.h"

class VentilationSolver {
public:
    // Computes pressure drops across airways and updates network flow imbalances
    static void computePressuresAndImbalances(VentilationGraph& graph);
};

#endif // SOLVER_H
