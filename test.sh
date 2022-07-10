#!/bin/bash

videoPath="./media/test_1s.mp4"
threshold="0.4"

./build/main $videoPath $threshold SEQUENTIAL PIPELINE_OFF
./build/main $videoPath $threshold SEQUENTIAL PIPELINE_ON

for nw in 1 2 4 8 16 32
do
    ./build/main $videoPath $threshold PARALLEL PIPELINE_OFF $nw
    ./build/main $videoPath $threshold PARALLEL PIPELINE_ON $nw
done

for nw in 1 2 4 8 16 32
do
    ./build/main $videoPath $threshold FASTFLOW PIPELINE_OFF $nw
    ./build/main $videoPath $threshold FASTFLOW PIPELINE_ON $nw
done