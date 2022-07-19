
rm -rf build
mkdir build

videoPath="./media/test_1m.mp4"
threshold="0.4"
type="SEQUENTIAL"

echo "NOT VECTORIZED"
g++ src/main.cpp -o build/main `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17
./build/main $videoPath $threshold $type

echo "VECTORIZED"
g++ src/main.cpp -O3 -o build/main `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17
./build/main $videoPath $threshold $type