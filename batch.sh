for i in 4 8 16 32 64 128 256 512 1024
do
    ./run_bench.sh $i 8 64
done

for i in 1 2 4 8 16
do
    ./run_bench.sh 64 $i 64
done

for i in 16 32 64 128
do
    ./run_bench.sh 64 8 $i
done

./run_plru.sh 64 8 64
