#!/bin/bash

make clean
make TARGET_LRU=lru

echo "$1 $2 $3" > config
mkdir ./output/"$1"_"$2"_"$3"

cd trace
for bench in *; do
    cd ../
    ./simulator ./trace/"$bench" > ./output/"$1"_"$2"_"$3"/"$bench"_"$1"_"$2"_"$3".out
    echo "${bench}_$1_$2_$3 bench done"
    cd trace
done
