#!/bin/bash

videoPath="./media/test_1s.mp4"
threshold="0.4"
formatter="CSV"

for spp in "SEQUENTIAL" "PARALLEL" "FASTFLOW"
do
    echo "${spp}_PIPELINE"
    echo "NW;1_GRAYSCALE;2_SMOOTHING;3_DETECT_DIFFERENCE;TOTAL_TIME;"
    ./build/main $videoPath $threshold SEQUENTIAL $spp 1 $formatter
    max=8
    if [[ $spp == "SEQUENTIAL" ]]
    then
        max=32
    fi
    for ((nw=2;nw<=$max;nw++))
    do
        ./build/main $videoPath $threshold PARALLEL $spp $nw $formatter
    done
    for ((nw=2;nw<=$max;nw++))
    do
        ./build/main $videoPath $threshold FASTFLOW $spp $nw $formatter
    done
done