#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <chrono>
#include <omp.h>
#include <iomanip> // for fixed and setprecision

using namespace std;
using namespace std::chrono;

class Graph {
    int V;
    vector<vector<int>> adj;

public:
    Graph(int V) : V(V), adj(V) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // undirected graph
    }

    void sequentialBFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;
        visited[start] = true;
        q.push(start);

        cout << "Sequential BFS: ";
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            cout << u << " ";

            for (int v : adj[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        cout << endl;
    }

    void parallelBFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;
        visited[start] = true;
        q.push(start);

        cout << "Parallel BFS: ";
        while (!q.empty()) {
            int size = q.size();
            vector<int> level(size);
            for (int i = 0; i < size; ++i) {
                level[i] = q.front();
                q.pop();
            }

            #pragma omp parallel for
            for (int i = 0; i < size; ++i) {
                int u = level[i];

                #pragma omp critical
                cout << u << " ";

                for (int v : adj[u]) {
                    if (!visited[v]) {
                        #pragma omp critical
                        {
                            if (!visited[v]) {
                                visited[v] = true;
                                q.push(v);
                            }
                        }
                    }
                }
            }
        }
        cout << endl;
    }

    void sequentialDFS(int start) {
        vector<bool> visited(V, false);
        stack<int> s;
        s.push(start);

        cout << "Sequential DFS: ";
        while (!s.empty()) {
            int u = s.top();
            s.pop();

            if (!visited[u]) {
                visited[u] = true;
                cout << u << " ";

                for (int i = adj[u].size() - 1; i >= 0; --i) {
                    int v = adj[u][i];
                    if (!visited[v]) {
                        s.push(v);
                    }
                }
            }
        }
        cout << endl;
    }

    void parallelDFS(int start) {
        vector<bool> visited(V, false);
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

            if (!visited[u]) {
                visited[u] = true;
                #pragma omp critical
                cout << u << " ";

                #pragma omp parallel for
                for (int i = 0; i < adj[u].size(); ++i) {
                    int v = adj[u][i];
                    if (!visited[v]) {
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
    int V, E, u, v, start;
    cout << "Enter number of vertices and edges: ";
    cin >> V >> E;

    Graph g(V);
    cout << "Enter edges (u v):" << endl;
    for (int i = 0; i < E; ++i) {
        cin >> u >> v;
        g.addEdge(u, v);
    }

    cout << "Enter starting node: ";
    cin >> start;

    cout << fixed << setprecision(0); // Format time in microseconds

    auto t1 = high_resolution_clock::now();
    g.sequentialBFS(start);
    auto t2 = high_resolution_clock::now();
    cout << "Sequential BFS Time: " << duration_cast<microseconds>(t2 - t1).count() << " microseconds\n";

    t1 = high_resolution_clock::now();
    g.parallelBFS(start);
    t2 = high_resolution_clock::now();
    cout << "Parallel BFS Time: " << duration_cast<microseconds>(t2 - t1).count() << " microseconds\n";

    t1 = high_resolution_clock::now();
    g.sequentialDFS(start);
    t2 = high_resolution_clock::now();
    cout << "Sequential DFS Time: " << duration_cast<microseconds>(t2 - t1).count() << " microseconds\n";

    t1 = high_resolution_clock::now();
    g.parallelDFS(start);
    t2 = high_resolution_clock::now();
    cout << "Parallel DFS Time: " << duration_cast<microseconds>(t2 - t1).count() << " microseconds\n";

    return 0;
}
