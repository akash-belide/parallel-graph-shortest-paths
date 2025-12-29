#include <bits/stdc++.h>
#include <omp.h>

using namespace std;

const long long INF = (long long)1e18;

// Load weighted directed graph from file: "n m" then lines "u v w"
vector<vector<pair<int,int>>> load_graph(const string& filename, int &n, int &m) {
    ifstream in(filename);
    if (!in) {
        cerr << "Error: could not open file " << filename << "\n";
        exit(1);
    }

    in >> n >> m;
    if (!in) {
        cerr << "Error: failed to read n and m from " << filename << "\n";
        exit(1);
    }

    vector<vector<pair<int,int>>> adj(n);
    int u, v, w;
    bool has_negative = false;

    for (int i = 0; i < m; ++i) {
        in >> u >> v >> w;
        if (!in) {
            cerr << "Error: failed to read edge " << i << " from " << filename << "\n";
            exit(1);
        }
        if (u < 0 || u >= n || v < 0 || v >= n) {
            cerr << "Warning: edge (" << u << "," << v << ") out of range, skipping.\n";
            continue;
        }
        if (w < 0) has_negative = true;
        adj[u].push_back({v, w});   // store edge u -> v with weight w
    }

    if (has_negative) {
        cerr << "Warning: graph has negative weights. Dijkstra is not valid for negative edges.\n";
    }

    return adj;
}

// Plain O(n^2) Dijkstra (no fancy priority queue)
// dist[v] = shortest distance from source to v
vector<long long> dijkstra_seq(const vector<vector<pair<int,int>>>& adj, int source) {
    int n = (int)adj.size();
    vector<long long> dist(n, INF);
    vector<char> used(n, 0);   // used[v] = 1 if v is already finalized

    dist[source] = 0;

    for (int it = 0; it < n; ++it) {
        int u = -1;
        long long best = INF;

        // find unused vertex with smallest dist
        for (int v = 0; v < n; ++v) {
            if (!used[v] && dist[v] < best) {
                best = dist[v];
                u = v;
            }
        }

        // no more reachable vertices
        if (u == -1 || best == INF) break;

        used[u] = 1;

        // relax all edges out of u
        for (auto &edge : adj[u]) {
            int to = edge.first;
            int w  = edge.second;
            if (dist[u] + w < dist[to]) {
                dist[to] = dist[u] + w;
            }
        }
    }

    return dist;
}

// "Parallel" O(n^2) Dijkstra
// Only the min-search is parallelized with OpenMP
vector<long long> dijkstra_parallel(const vector<vector<pair<int,int>>>& adj,
                                    int source, int num_threads) {
    int n = (int)adj.size();
    vector<long long> dist(n, INF);
    vector<char> used(n, 0);

    dist[source] = 0;
    omp_set_num_threads(num_threads);

    for (int it = 0; it < n; ++it) {
        int u = -1;
        long long best = INF;

        // parallel search for vertex with smallest dist and not used
        #pragma omp parallel
        {
            int local_u = -1;
            long long local_best = INF;

            #pragma omp for nowait
            for (int v = 0; v < n; ++v) {
                if (!used[v] && dist[v] < local_best) {
                    local_best = dist[v];
                    local_u = v;
                }
            }

            // pick global best among all threads
            #pragma omp critical
            {
                if (local_best < best) {
                    best = local_best;
                    u = local_u;
                }
            }
        }

        // no more reachable vertices
        if (u == -1 || best == INF) break;

        used[u] = 1;

        // relax all edges out of u (sequential for simplicity)
        for (auto &edge : adj[u]) {
            int to = edge.first;
            int w  = edge.second;
            long long cand = dist[u] + (long long)w;
            if (cand < dist[to]) {
                dist[to] = cand;
            }
        }
    }

    return dist;
}

// Print how many nodes we reached and max distance
void summarize(const vector<long long>& dist) {
    int n = (int)dist.size();
    int reached = 0;
    long long maxd = 0;
    for (int i = 0; i < n; ++i) {
        if (dist[i] < INF) {
            reached++;
            if (dist[i] > maxd) maxd = dist[i];
        }
    }
    cout << "  Reached " << reached << " / " << n << " nodes.\n";
    cout << "  Max finite distance from source = " << maxd << "\n";
}

// Entry point: choose seq vs parallel based on num_threads
int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <graph_file> <num_threads>\n";
        cerr << "  num_threads = 0 for sequential Dijkstra\n";
        cerr << "              = 1,2,4,8,... for parallel Dijkstra\n";
        return 1;
    }

    string filename = argv[1];
    int num_threads = stoi(argv[2]);

    int n, m;
    auto adj = load_graph(filename, n, m);
    int source = 0;   // we always start from node 0

    cout << "Graph: " << filename << " (n=" << n << ", m=" << m << ")\n";
    cout << "Source vertex: " << source << "\n";

    double t0 = omp_get_wtime();
    vector<long long> dist;

    if (num_threads == 0) {
        cout << "Running sequential Dijkstra...\n";
        dist = dijkstra_seq(adj, source);
    } else {
        cout << "Running parallel Dijkstra with " << num_threads << " threads...\n";
        dist = dijkstra_parallel(adj, source, num_threads);
    }

    double t1 = omp_get_wtime();
    cout << "Time: " << (t1 - t0) << " seconds\n";
    summarize(dist);

    return 0;
}