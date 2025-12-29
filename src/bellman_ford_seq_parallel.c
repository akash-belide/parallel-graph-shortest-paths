#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

const long long INF = (long long)1e18;

// simple edge struct for Bellman-Ford
struct Edge {
    int u, v;
    int w;
};

// load graph from "n m" followed by "u v w"
vector<Edge> load_graph(const string& filename, int &n, int &m) {
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

    vector<Edge> edges;
    edges.reserve(m);

    int u, v, w;
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
        edges.push_back({u, v, w});
    }

    // update m in case we skipped any bad edges
    m = (int)edges.size();
    return edges;
}

// plain sequential Bellman-Ford
vector<long long> bellman_ford_seq(const vector<Edge>& edges, int n, int source) {
    vector<long long> dist(n, INF);
    dist[source] = 0;

    // relax edges up to n-1 times
    for (int it = 0; it < n - 1; ++it) {
        bool changed = false;

        for (const auto &e : edges) {
            if (dist[e.u] == INF) continue;

            long long cand = dist[e.u] + (long long)e.w;
            if (cand < dist[e.v]) {
                dist[e.v] = cand;
                changed = true;
            }
        }

        // no change means we are done
        if (!changed) break;
    }

    return dist;
}

// parallel Bellman-Ford with per-vertex locks
vector<long long> bellman_ford_par(const vector<Edge>& edges, int n,
                                   int source, int threads) {
    vector<long long> dist(n, INF);
    dist[source] = 0;
    
    // Create one lock per vertex to avoid global serialization
    vector<omp_lock_t> locks(n);
    for (int i = 0; i < n; i++) {
        omp_init_lock(&locks[i]);
    }
    
    omp_set_num_threads(threads);
    
    for (int it = 0; it < n - 1; ++it) {
        bool updated = false;
        
        #pragma omp parallel for reduction(||:updated) schedule(dynamic, 256)
        for (int i = 0; i < (int)edges.size(); ++i) {
            const Edge &e = edges[i];
            
            // Read source distance (no lock needed for reading)
            long long dist_u = dist[e.u];
            if (dist_u == INF) continue;
            
            long long cand = dist_u + (long long)e.w;
            
            // Quick check before locking (avoid unnecessary lock contention)
            if (cand >= dist[e.v]) continue;
            
            // Lock only the target vertex we're updating
            omp_set_lock(&locks[e.v]);
            
            // Double-check after acquiring lock (another thread might have updated)
            if (cand < dist[e.v]) {
                dist[e.v] = cand;
                updated = true;
            }
            
            omp_unset_lock(&locks[e.v]);
        }
        
        // if nothing improved, stop early
        if (!updated) break;
    }
    
    // Clean up locks
    for (int i = 0; i < n; i++) {
        omp_destroy_lock(&locks[i]);
    }
    
    return dist;
}

// check for negative weight cycle using final dist
bool has_negative_cycle(const vector<Edge>& edges,
                        const vector<long long>& dist) {
    for (const auto &e : edges) {
        if (dist[e.u] == INF) continue;
        long long cand = dist[e.u] + (long long)e.w;
        if (cand < dist[e.v]) {
            return true;
        }
    }
    return false;
}

// small summary: how many nodes we reached and max distance
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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <graph_file> <num_threads>\n";
        cerr << "  num_threads = 0 for sequential Bellman-Ford\n";
        cerr << "              = 1,2,4,8,... for parallel Bellman-Ford\n";
        return 1;
    }

    string filename = argv[1];
    int num_threads = stoi(argv[2]);

    int n, m;
    auto edges = load_graph(filename, n, m);
    int source = 0;  // start from node 0

    cout << "Graph: " << filename << " (n=" << n << ", m=" << m << ")\n";
    cout << "Source vertex: " << source << "\n";

    double t0 = omp_get_wtime();
    vector<long long> dist;

    if (num_threads == 0) {
        cout << "Running sequential Bellman-Ford...\n";
        dist = bellman_ford_seq(edges, n, source);
    } else {
        cout << "Running parallel Bellman-Ford with " << num_threads << " threads...\n";
        dist = bellman_ford_par(edges, n, source, num_threads);
    }

    double t1 = omp_get_wtime();
    cout << "Time: " << (t1 - t0) << " seconds\n";

    summarize(dist);

    // check for negative cycles
    if (has_negative_cycle(edges, dist)) {
        cout << "  Warning: graph contains a negative weight cycle.\n";
        cout << "  Shortest-path distances are not well-defined.\n";
    } else {
        cout << "  No negative weight cycle detected.\n";
    }

    return 0;
}