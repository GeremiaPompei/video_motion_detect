#!/bin/bash

videoPath="./media/test_1m.mp4"
threshold="0.8"
formatter="CSV"

echo "INFO;NW;1_GRAYSCALE;2_SMOOTHING;3_DETECT_DIFFERENCE;TOTAL_TIME;"
./build/main $videoPath $threshold SEQUENTIAL 1 $formatter
max=32
for ((nw=2;nw<=$max;nw++))
do
    ./build/main $videoPath $threshold NATIVE_THREADS $nw $formatter
done
for ((nw=2;nw<=$max;nw++))
do
    ./build/main $videoPath $threshold FASTFLOW $nw $formatter
done