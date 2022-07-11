#!/bin/bash

videoPath="./media/test_1s.mp4"
threshold="0.4"
formatter="CSV"

for spp in SEQUENTIAL PARALLEL FASTFLOW
do
    echo "${spp}_PIPELINE"
    echo "1_GRAYSCALE;2_SMOOTHING;3_DETECT_DIFFERENCE;TOTAL_TIME;"
    ./build/main $videoPath $threshold SEQUENTIAL $spp 1 $formatter
    for nw in 1 2 4 8 16 32
    do
        ./build/main $videoPath $threshold PARALLEL $spp $nw $formatter
    done
    for nw in 1 2 4 8 16 32
    do
        ./build/main $videoPath $threshold FASTFLOW $spp $nw $formatter
    done
done