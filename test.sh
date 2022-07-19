#!/bin/bash

videoPath="./media/test_1m.mp4"
threshold="0.8"
formatter=""

# ./build/main $videoPath $threshold SEQUENTIAL 1 $formatter

for nw in 4 # 1 2 3 4 5 6 7 8
do
    ./build/main $videoPath $threshold NATIVE_THREADS $nw $formatter
done

for nw in 4 # 1 2 3 4 5 6 7 8
do
    ./build/main $videoPath $threshold FASTFLOW $nw $formatter
done