#include <iostream>
#include <vector>
#include <stack>
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

    // Sequential DFS
    void dfs(int start)
    {
        resetVisited();

        stack<int> s;
        s.push(start);

        visited[start] = true;

        while (!s.empty())
        {
            int current = s.top();
            s.pop();

            cout << current << " ";

            for (int i = 0; i < graph[current].size(); i++)
            {
                int neighbor = graph[current][i];

                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    s.push(neighbor);
                }
            }
        }
    }

    // Parallel DFS
    void parallel_dfs(int start)
    {
        resetVisited();

        #pragma omp parallel
        {
            #pragma omp single
            dfs_task(start);
        }
    }

    void dfs_task(int start)
    {
        stack<int> s;
        s.push(start);

        visited[start] = true;

        while (!s.empty())
        {
            int current = s.top();
            s.pop();

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
                            s.push(neighbor);
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

    cout << "\nSequential DFS:\n";

    auto start = high_resolution_clock::now();

    g.dfs(0);

    auto end = high_resolution_clock::now();

    double time1 = duration<double>(end - start).count();

    cout << "\nTime: " << time1 << " seconds\n";

    cout << "\nParallel DFS:\n";

    start = high_resolution_clock::now();

    g.parallel_dfs(0);

    end = high_resolution_clock::now();

    double time2 = duration<double>(end - start).count();

    cout << "\nTime: " << time2 << " seconds\n";

    cout << "Speedup (DFS): " << time1 / time2 << "\n";

    return 0;
}


/* Commands-
1st -  g++ DFS.cpp -fopenmp -o DFS
2nd - ./DFS
*/