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

    void addEdge(int v, int w) {
        adj[v].push_back(w);
        adj[w].push_back(v);
    }

    void printGraph() {
        cout << "\nGraph Adjacency List:\n";
        for (int i = 0; i < V; i++) {
            cout << "Vertex " << i << ": ";
            for (int neighbor : adj[i]) {
                cout << neighbor << " ";
            }
            cout << endl;
        }
    }

    void parallelBFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;

        double start_time = omp_get_wtime();

        visited[start] = true;
        q.push(start);

        cout << "\nParallel BFS starting from vertex " << start << ":\n";

        while (!q.empty()) {
            #pragma omp parallel
            {
                #pragma omp single
                {
                    int level_size = q.size();
                    for (int i = 0; i < level_size; i++) {
                        int v = q.front();
                        q.pop();
                        cout << v << " ";

                        #pragma omp task firstprivate(v) shared(visited, q)
                        {
                            for (int neighbor : adj[v]) {
                                #pragma omp critical
                                {
                                    if (!visited[neighbor]) {
                                        visited[neighbor] = true;
                                        q.push(neighbor);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        double end_time = omp_get_wtime();
        cout << "\n\nParallel BFS completed in " << (end_time - start_time) * 1000 << " milliseconds\n";
    }

    void parallelDFS(int start) {
        vector<bool> visited(V, false);
        stack<int> s;

        double start_time = omp_get_wtime();

        s.push(start);
        visited[start] = true;

        cout << "\nParallel DFS starting from vertex " << start << ":\n";

        while (!s.empty()) {
            int v;
            #pragma omp critical
            {
                v = s.top();
                s.pop();
            }

            cout << v << " ";

            #pragma omp parallel for
            for (int i = 0; i < adj[v].size(); i++) {
                int neighbor = adj[v][i];
                #pragma omp critical
                {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        s.push(neighbor);
                    }
                }
            }
        }

        double end_time = omp_get_wtime();
        cout << "\n\nParallel DFS completed in " << (end_time - start_time) * 1000 << " milliseconds\n";
    }
};

int main() {
    int V, E, start_vertex;
    int num_threads;

    cout << "Enter number of vertices: ";
    cin >> V;
    cout << "Enter number of edges: ";
    cin >> E;

    Graph g(V);

    cout << "Enter edges (vertex pairs, 0-based indexing):\n";
    for (int i = 0; i < E; i++) {
        int v, w;
        cin >> v >> w;
        if (v >= V || w >= V || v < 0 || w < 0) {
            cout << "Invalid vertex! Vertices must be between 0 and " << V - 1 << endl;
            i--;
            continue;
        }
        g.addEdge(v, w);
    }

    cout << "Enter starting vertex for BFS/DFS (0-" << V - 1 << "): ";
    cin >> start_vertex;
    if (start_vertex < 0 || start_vertex >= V) {
        cout << "Invalid starting vertex! Using 0 as default.\n";
        start_vertex = 0;
    }

    cout << "Enter number of threads to use: ";
    cin >> num_threads;
    omp_set_num_threads(num_threads);

    g.printGraph();

    g.parallelBFS(start_vertex);
    g.parallelDFS(start_vertex);

    return 0;
}

