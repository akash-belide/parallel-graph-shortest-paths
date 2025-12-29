(
for n in 100 500 1000 5000 10000 50000 100000 500000 1000000; do
    echo "======================================="
    echo "Generating graph with n=$n"
    ./generate_graph $n 8 10 1 45    # 1 -> includes some negative weights

    for t in 0 1 2 4 8; do
        echo "---------------------------------------"
        echo "Running Bellman-Ford on graph_$n.txt with threads=$t"
        ./bellman_ford_seq_parallel graph_${n}.txt $t
    done
done
) 2>&1 | tee bellman_ford_results.log
