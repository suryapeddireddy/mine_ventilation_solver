#include <bits/stdc++.h>
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
    for (int i = 0; i < m; i++) {
        fin >> u[i] >> v[i] >> R[i] >> F[i];
        if (u[i] < 0 || u[i] >= n || v[i] < 0 || v[i] >= n || R[i] <= 0) {
            cout << "bad airway " << i << "\n";
            return 1;
        }
    }

    cout << n << " nodes, " << m << " airways\n";
    for (int i = 0; i < m; i++)
        cout << i << ": " << u[i] << " -> " << v[i] << "  R=" << R[i] << "  fan=" << F[i] << "\n";

    // spanning tree with union-find
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

    cout << "tree airways:";
    for (int i = 0; i < m; i++)
        if (inTree[i]) cout << " " << i;
    cout << "\n";

    cout << "non-tree airways (one loop each):";
    for (int i = 0; i < m; i++)
        if (!inTree[i]) cout << " " << i;
    cout << "\n";

    cout << "number of loops = m - n + 1 = " << m - n + 1 << "\n";

    // tree adjacency: adj[node] = list of (neighbor, airway index)
    vector<vector<pair<int,int>>> adj(n);
    for (int i = 0; i < m; i++) {
        if (inTree[i]) {
            adj[u[i]].push_back({v[i], i});
            adj[v[i]].push_back({u[i], i});
        }
    }

    // BFS from node 0 to get parent, parent airway, depth
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

    // build one loop per non-tree airway
    vector<vector<pair<int,int>>> loops;   // each item: (airway, +1 or -1)
    for (int e = 0; e < m; e++) {
        if (inTree[e]) continue;
        vector<pair<int,int>> loop, down;
        loop.push_back({e, +1});           // travel e from u[e] to v[e]
        int x = v[e], y = u[e];
        while (x != y) {
            if (depth[x] >= depth[y]) {    // climb from x toward the root
                int pe = parentEdge[x];
                loop.push_back({pe, (u[pe] == x) ? +1 : -1});
                x = parent[x];
            } else {                       // climb from y; walked downward later
                int pe = parentEdge[y];
                down.push_back({pe, (u[pe] == parent[y]) ? +1 : -1});
                y = parent[y];
            }
        }
        reverse(down.begin(), down.end());
        for (auto p : down) loop.push_back(p);
        loops.push_back(loop);
    }

    for (int k = 0; k < (int)loops.size(); k++) {
        cout << "loop " << k << ":";
        for (auto p : loops[k])
            cout << " " << p.first << (p.second == 1 ? "(+)" : "(-)");
        cout << "\n";
    }
    return 0;
}