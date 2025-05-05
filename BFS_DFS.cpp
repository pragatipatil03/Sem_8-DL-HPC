#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>
using namespace std;

class Graph {
    int V;
    vector<vector<int>> adj;

public:
    Graph(int V) : V(V), adj(V) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // Assuming undirected graph
    }

    void parallelBFS(int start) {
        vector<bool> vis(V, false);
        queue<int> q;
        vis[start] = true;
        q.push(start);
        cout << "Parallel BFS: ";
        while (!q.empty()) {
            int n = q.size();
            vector<int> level(n);
            for (int i = 0; i < n; ++i) {
                level[i] = q.front();
                q.pop();
            }
            #pragma omp parallel for
            for (int i = 0; i < n; ++i) {
                int u = level[i];
                #pragma omp critical
                cout << u << " ";
                for (int v : adj[u]) {
                    if (!vis[v]) {
                        #pragma omp critical
                        {
                            if (!vis[v]) {
                                vis[v] = true;
                                q.push(v);
                            }
                        }
                    }
                }
            }
        }
        cout << endl;
    }

    void parallelDFS(int start) {
        vector<bool> vis(V, false);
        stack<int> s;
        s.push(start);
        cout << "Parallel DFS: ";
        while (!s.empty()) {
            int u;
            #pragma omp critical
            {
                u = s.top();
                s.pop();
            }
            if (!vis[u]) {
                vis[u] = true;
                #pragma omp critical
                cout << u << " ";
                #pragma omp parallel for
                for (int i = 0; i < adj[u].size(); ++i) {
                    int v = adj[u][i];
                    if (!vis[v]) {
                        #pragma omp critical
                        s.push(v);
                    }
                }
            }
        }
        cout << endl;
    }
};

int main() {
    int V, E;
    cout << "Enter number of vertices: ";
    cin >> V;
    cout << "Enter number of edges: ";
    cin >> E;

    Graph g(V);
    cout << "Enter edges (u v):" << endl;
    for (int i = 0; i < E; ++i) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }

    int start;
    cout << "Enter starting node: ";
    cin >> start;

    g.parallelBFS(start);
    g.parallelDFS(start);

    return 0;
}
