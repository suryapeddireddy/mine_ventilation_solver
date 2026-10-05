#include <bits/stdc++.h>
#include "graph.h"
#include "solver.h"
#include "hpc_check.h"

using namespace std;

int n, m;
vector<int> u, v;
vector<double> R, F;
vector<int> par;

int find(int x) {
    if (par[x] == x) return x;
    par[x] = find(par[x]);
    return par[x];
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "usage: ./vent file\n";
        return 1;
    }
    ifstream fin(argv[1]);
    if (!fin) {
        cout << "cannot open file\n";
        return 1;
    }

    fin >> n >> m;
    u.resize(m); v.resize(m); R.resize(m); F.resize(m);
    
    // Create the OOP graph infrastructure
    VentilationGraph network(n);
    for (int i = 0; i < n; i++) {
        network.addJunction(i, "Junction_" + to_string(i));
    }

    for (int i = 0; i < m; i++) {
        fin >> u[i] >> v[i] >> R[i] >> F[i];
        if (u[i] < 0 || u[i] >= n || v[i] < 0 || v[i] >= n || R[i] <= 0) {
            cout << "bad airway " << i << "\n";
            return 1;
        }
        // Synergize: Populate the graph with initial flows (e.g., 10.0 m3/s baseline)
        network.addAirway(i, u[i], v[i], R[i], 10.0);
    }

    cout << n << " nodes, " << m << " airways loaded.\n";

    // Spanning tree extraction via Union-Find Disjoint Sets
    par.resize(n);
    for (int i = 0; i < n; i++) par[i] = i;

    vector<bool> inTree(m, false);
    int treeEdges = 0;
    for (int i = 0; i < m; i++) {
        int a = find(u[i]);
        int b = find(v[i]);
        if (a != b) {
            par[a] = b;
            inTree[i] = true;
            treeEdges++;
        }
    }

    if (treeEdges != n - 1) {
        cout << "network is not connected\n";
        return 1;
    }

    // Build the fundamental tree adjacency list for BFS loop detection
    vector<vector<pair<int,int>>> adj(n);
    for (int i = 0; i < m; i++) {
        if (inTree[i]) {
            adj[u[i]].push_back({v[i], i});
            adj[v[i]].push_back({u[i], i});
        }
    }

    vector<int> parent(n, -1), parentEdge(n, -1), depth(n, 0);
    vector<bool> seen(n, false);
    queue<int> q;
    q.push(0);
    seen[0] = true;
    while (!q.empty()) {
        int x = q.front();
        q.pop();
        for (auto p : adj[x]) {
            int y = p.first, e = p.second;
            if (!seen[y]) {
                seen[y] = true;
                parent[y] = x;
                parentEdge[y] = e;
                depth[y] = depth[x] + 1;
                q.push(y);
            }
        }
    }

    // Extract loop paths (+1 same direction, -1 opposing direction)
    vector<vector<pair<int,int>>> fundamental_loops;
    for (int e = 0; e < m; e++) {
        if (inTree[e]) continue;
        vector<pair<int,int>> loop, down;
        loop.push_back({e, +1});
        int x = v[e], y = u[e];
        while (x != y) {
            if (depth[x] >= depth[y]) {
                int pe = parentEdge[x];
                loop.push_back({pe, (u[pe] == x) ? +1 : -1});
                x = parent[x];
            } else {
                int pe = parentEdge[y];
                down.push_back({pe, (u[pe] == parent[y]) ? +1 : -1});
                y = parent[y];
            }
        }
        reverse(down.begin(), down.end());
        for (auto p : down) loop.push_back(p);
        fundamental_loops.push_back(loop);
    }

    // 1. Run Hardy Cross Engine 
    cout << "\n=== Launching Numerical Solver Pipeline ===\n";
    VentilationSolver::computePressuresAndImbalances(network);

    // 2. Trigger Multi-threaded HPC reduction validation check
    cout << "\n=== Running Parallel HPC Diagnostics ===\n";
    int threads_to_use = 4;
    HPCCheck::verifyContinuityParallel(network, threads_to_use);

    return 0;
}
