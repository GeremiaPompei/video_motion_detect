#!/bin/bash

videoPath="./media/test_1s.mp4"
threshold="0.4"
formatter="CSV"

./build/main $videoPath $threshold SEQUENTIAL SEQUENTIAL 1 $formatter
./build/main $videoPath $threshold SEQUENTIAL PARALLEL 1 $formatter
./build/main $videoPath $threshold SEQUENTIAL FASTFLOW 1 $formatter

for nw in 1 2 4 8 16 32
do
    ./build/main $videoPath $threshold PARALLEL SEQUENTIAL $nw $formatter
    if (("$nw" <= "8"))
    then
        ./build/main $videoPath $threshold PARALLEL PARALLEL $nw $formatter
        ./build/main $videoPath $threshold PARALLEL FASTFLOW $nw $formatter
    fi
done

for nw in 1 2 4 8 16 32
do
    ./build/main $videoPath $threshold FASTFLOW SEQUENTIAL $nw $formatter
    if (("$nw" <= "8"))
    then
        ./build/main $videoPath $threshold FASTFLOW PARALLEL $nw $formatter
        ./build/main $videoPath $threshold FASTFLOW FASTFLOW $nw $formatter
    fi
done