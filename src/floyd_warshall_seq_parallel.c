#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

const long long INF = (long long)1e15;

// build n x n matrix from "n m" + edges file
vector<vector<long long>> load_matrix(const string& file, int &n, int &m) {
    ifstream in(file);
    if (!in) {
        cerr << "Error: could not open file " << file << "\n";
        exit(1);
    }

    in >> n >> m;
    if (!in) {
        cerr << "Error: failed to read n and m from " << file << "\n";
        exit(1);
    }

    vector<vector<long long>> dist(n, vector<long long>(n, INF));

    // distance to self is 0
    for (int i = 0; i < n; i++) dist[i][i] = 0;

    int u, v, w;
    for (int i = 0; i < m; i++) {
        in >> u >> v >> w;
        if (!in) {
            cerr << "Error: failed to read edge " << i << " from " << file << "\n";
            exit(1);
        }
        if (u < 0 || u >= n || v < 0 || v >= n) {
            cerr << "Warning: edge (" << u << "," << v << ") out of range, skipping.\n";
            continue;
        }
        long long ww = w;
        // keep the smallest edge in case of duplicates
        dist[u][v] = min(dist[u][v], ww);
    }

    return dist;
}

// plain sequential Floyd–Warshall
vector<vector<long long>> floyd_warshall_seq(vector<vector<long long>> dist) {
    int n = (int)dist.size();

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            if (dist[i][k] == INF) continue;
            for (int j = 0; j < n; j++) {
                if (dist[k][j] == INF) continue;
                long long cand = dist[i][k] + dist[k][j];
                if (cand < dist[i][j]) {
                    dist[i][j] = cand;
                }
            }
        }
    }

    return dist;
}

// parallel Floyd–Warshall: parallel over rows i for each k
vector<vector<long long>> floyd_warshall_par(vector<vector<long long>> dist,
                                             int threads) {
    int n = (int)dist.size();
    omp_set_num_threads(threads);

    for (int k = 0; k < n; k++) {
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            if (dist[i][k] == INF) continue;
            for (int j = 0; j < n; j++) {
                if (dist[k][j] == INF) continue;
                long long cand = dist[i][k] + dist[k][j];
                if (cand < dist[i][j]) {
                    dist[i][j] = cand;
                }
            }
        }
    }

    return dist;
}

// check if any dist[i][i] < 0
bool has_negative_cycle(const vector<vector<long long>>& dist) {
    int n = (int)dist.size();
    for (int i = 0; i < n; i++) {
        if (dist[i][i] < 0) return true;
    }
    return false;
}

// quick summary about matrix
void summarize(const vector<vector<long long>>& dist) {
    int n = (int)dist.size();
    int reachable = 0;
    long long maxd = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (dist[i][j] < INF) {
                reachable++;
                if (dist[i][j] > maxd) maxd = dist[i][j];
            }
        }
    }

    cout << "  Reachable pairs: " << reachable << " / " << (long long)n * n << "\n";
    cout << "  Max finite distance: " << maxd << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <graph_file> <num_threads>\n";
        cerr << "  num_threads = 0 for sequential Floyd–Warshall\n";
        cerr << "              = 1,2,4,8,... for parallel Floyd–Warshall\n";
        return 1;
    }

    string file = argv[1];
    int threads = stoi(argv[2]);

    int n, m;
    auto dist0 = load_matrix(file, n, m);

    cout << "Graph: " << file << " (n=" << n << ", m=" << m << ")\n";

    if (n > 2000) {
        cout << "Warning: Floyd–Warshall is O(n^3). n=" << n
             << " may be too large.\n";
    }

    double t0 = omp_get_wtime();
    vector<vector<long long>> dist;

    if (threads == 0) {
        cout << "Running sequential Floyd–Warshall...\n";
        dist = floyd_warshall_seq(dist0);
    } else {
        cout << "Running parallel Floyd–Warshall with " << threads << " threads...\n";
        dist = floyd_warshall_par(dist0, threads);
    }

    double t1 = omp_get_wtime();
    cout << "Time: " << (t1 - t0) << " seconds\n";

    summarize(dist);

    if (has_negative_cycle(dist)) {
        cout << "  Warning: graph contains a negative cycle.\n";
    } else {
        cout << "  No negative cycle detected.\n";
    }

    return 0;
}