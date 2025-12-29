// Compile:
//   gcc -O2 -Wall -std=c99 gen_graph.c -o gen_graph
//
// Run:
//   ./generate_graph <n> <avgDeg> <maxWeight> <negFlag> <seed>
//
// This will create: graph_<n>.txt
//
// Format of output file:
//   First line: n m
//   Then m lines: u v w
//
// - negFlag = 0 -> weights in [1, maxWeight] (all positive)
// - negFlag = 1 -> some negative edges, graph is a DAG (no negative cycles).
// - Use negFlag=0 for Dijkstra; negFlag=1 for Bellman-Ford / Floyd-Warshall tests.

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int u, v, w;
} Edge;

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr,
            "Usage: %s <n> <avgDeg> <maxWeight> <negFlag> <seed>\n"
            "  negFlag = 0 (positive only) or 1 (include negatives, DAG)\n",
            argv[0]);
        return 1;
    }

    int n       = atoi(argv[1]);
    int avgDeg  = atoi(argv[2]);
    int maxW    = atoi(argv[3]);
    int negFlag = atoi(argv[4]);
    unsigned int seed = (unsigned int)strtoul(argv[5], NULL, 10);

    if (n <= 0 || avgDeg <= 0 || maxW <= 0) {
        fprintf(stderr, "Error: n, avgDeg, and maxWeight must be > 0\n");
        return 1;
    }

    // Decide how many edges we want
    long long target_m_ll = (long long)n * avgDeg;
    if (target_m_ll < n - 1) target_m_ll = n - 1; // at least a tree

    if (target_m_ll > 2000000000LL) {
        fprintf(stderr, "Error: too many edges requested.\n");
        return 1;
    }
    int m = (int)target_m_ll;

    // Allocate edge array
    Edge *edges = (Edge *)malloc((size_t)m * sizeof(Edge));
    if (!edges) {
        fprintf(stderr, "Error: could not allocate memory for edges.\n");
        return 1;
    }

    srand(seed);

    int idx = 0;

    // 1) Spanning tree to ensure connectivity
    //
    // For negFlag = 1, this also ensures u < v (so part of the DAG property).
    for (int v = 1; v < n; ++v) {
        int u;
        if (negFlag == 0) {
            // parent can be any node 0..v-1 (still u < v)
            u = rand() % v;
        } else {
            // also u in 0..v-1 ensures u < v, keeping graph acyclic
            u = rand() % v;
        }

        int base = (rand() % maxW) + 1;
        int w;

        if (negFlag == 0) {
            w = base; // positive
        } else {
            // 80% positive, 20% negative
            int positive = (rand() % 10) >= 2;
            w = positive ? base : -base;
        }

        edges[idx].u = u;
        edges[idx].v = v;
        edges[idx].w = w;
        idx++;
    }

    // 2) Extra edges up to target m
    while (idx < m) {
        int u, v;

        if (negFlag == 0) {
            // no restriction for positive-only graph
            u = rand() % n;
            v = rand() % n;
            if (u == v) continue; // avoid self-loops
        } else {
            // DAG: enforce u < v
            if (n < 2) break;
            u = rand() % (n - 1);
            v = u + 1 + (rand() % (n - u - 1));
        }

        int base = (rand() % maxW) + 1;
        int w;
        if (negFlag == 0) {
            w = base;
        } else {
            int positive = (rand() % 10) >= 2;
            w = positive ? base : -base;
        }

        edges[idx].u = u;
        edges[idx].v = v;
        edges[idx].w = w;
        idx++;
    }

    // 3) Write to file graph_<n>.txt
    char filename[64];
    snprintf(filename, sizeof(filename), "graph_%d.txt", n);

    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("fopen");
        free(edges);
        return 1;
    }

    fprintf(f, "%d %d\n", n, m);
    for (int i = 0; i < m; ++i) {
        fprintf(f, "%d %d %d\n", edges[i].u, edges[i].v, edges[i].w);
    }

    fclose(f);
    free(edges);

    printf("Generated graph with %d nodes and %d edges into %s\n", n, m, filename);
    return 0;
}