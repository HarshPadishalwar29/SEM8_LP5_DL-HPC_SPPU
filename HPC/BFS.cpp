#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace chrono;

class Graph
{
public:
    int vertices;
    vector<vector<int>> graph;
    vector<bool> visited;

    Graph(int v)
    {
        vertices = v;
        graph.resize(v);
        visited.resize(v, false);
    }

    void addEdge(int a, int b)
    {
        graph[a].push_back(b);
        graph[b].push_back(a);
    }

    void resetVisited()
    {
        for (int i = 0; i < vertices; i++)
        {
            visited[i] = false;
        }
    }

    // Sequential BFS
    void bfs(int start)
    {
        resetVisited();

        queue<int> q;
        q.push(start);

        visited[start] = true;

        while (!q.empty())
        {
            int current = q.front();
            q.pop();

            cout << current << " ";

            for (int i = 0; i < graph[current].size(); i++)
            {
                int neighbor = graph[current][i];

                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
    }

    // Parallel BFS
    void parallel_bfs(int start)
    {
        resetVisited();

        #pragma omp parallel
        {
            #pragma omp single
            bfs_task(start);
        }
    }

    void bfs_task(int start)
    {
        queue<int> q;
        q.push(start);

        visited[start] = true;

        while (!q.empty())
        {
            int current = q.front();
            q.pop();

            cout << current << " ";

            #pragma omp parallel for
            for (int i = 0; i < graph[current].size(); i++)
            {
                int neighbor = graph[current][i];

                if (!visited[neighbor])
                {
                    #pragma omp critical
                    {
                        if (!visited[neighbor])
                        {
                            visited[neighbor] = true;
                            q.push(neighbor);
                        }
                    }
                }
            }
        }
    }
};

int main()
{
    int V = 50;
    int E = 1000;

    Graph g(V);

    for (int i = 0; i < E; i++)
    {
        int a = rand() % V;
        int b = rand() % V;

        if (a != b)
        {
            g.addEdge(a, b);
        }
    }

    cout << "\nSequential BFS:\n";

    auto start = high_resolution_clock::now();

    g.bfs(0);

    auto end = high_resolution_clock::now();

    double time1 = duration<double>(end - start).count();

    cout << "\nTime: " << time1 << " seconds\n";

    cout << "\nParallel BFS:\n";

    start = high_resolution_clock::now();

    g.parallel_bfs(0);

    end = high_resolution_clock::now();

    double time2 = duration<double>(end - start).count();

    cout << "\nTime: " << time2 << " seconds\n";

    cout << "Speedup (BFS): " << time1 / time2 << "\n";

    return 0;
}


/* Commands-
1st -  g++ BFS.cpp -fopenmp -o BFS
2nd - ./BFS
*/