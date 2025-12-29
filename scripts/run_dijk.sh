(
for n in 100 500 1000 5000 10000 50000 100000; do
    echo "======================================="
    echo "Generating graph with n=$n"
    ./generate_graph $n 8 10 0 45

    for t in 0 1 2 4 8; do
        echo "---------------------------------------"
        echo "Running Dijkstra on graph_$n.txt with threads=$t"
        ./dijkstra_seq_parallel graph_${n}.txt $t
    done
done
) 2>&1 | tee dijkstra_results.log
