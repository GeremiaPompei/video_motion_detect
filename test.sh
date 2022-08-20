#!/bin/bash

videoPath="./media/test_1m.mp4"
threshold="0.8"
formatter=""

for ((nw=30;nw>=2;nw--))
do
    ./build/main $videoPath $threshold NATIVE_THREADS $nw $formatter
    ./build/main $videoPath $threshold FASTFLOW $nw $formatter
done

./build/main $videoPath $threshold SEQUENTIAL 1 $formatter