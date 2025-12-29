(
for n in 100 500 1000 2000 5000 10000; do
    echo "======================================="
    echo "Generating graph with n=$n"
    ./generate_graph $n 8 10 1 45    # 1 -> allows some negative weights

    for t in 0 1 2 4 8; do
        echo "---------------------------------------"
        echo "Running Floyd-Warshall on graph_$n.txt with threads=$t"
        ./floyd_warshall_seq_parallel graph_${n}.txt $t
    done
done
) 2>&1 | tee floyd_warshall_results.log

