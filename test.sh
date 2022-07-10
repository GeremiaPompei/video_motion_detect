#!/bin/bash

videoPath="./media/test_1s.mp4"
threshold="0.4"

./build/main $videoPath $threshold SEQUENTIAL SEQUENTIAL
./build/main $videoPath $threshold SEQUENTIAL PARALLEL
./build/main $videoPath $threshold SEQUENTIAL FASTFLOW

for nw in 1 2 4 8 16 32
do
    ./build/main $videoPath $threshold PARALLEL SEQUENTIAL $nw
    ./build/main $videoPath $threshold PARALLEL PARALLEL $nw
    ./build/main $videoPath $threshold PARALLEL FASTFLOW $nw
done

for nw in 1 2 4 8 16 32
do
    ./build/main $videoPath $threshold FASTFLOW SEQUENTIAL $nw
    ./build/main $videoPath $threshold FASTFLOW PARALLEL $nw
    ./build/main $videoPath $threshold FASTFLOW FASTFLOW $nw
done