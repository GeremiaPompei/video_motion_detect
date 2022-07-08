# RUN AND COMPILE LOCALLY
rm -rf build
mkdir build
#g++-10 src/main.cpp -o build/main `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17
g++ src/main.cpp -o build/main `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17
./build/main ./media/test_1s.mp4 0.42

# RUN AND COMPILE DOCKER
# docker build -t mvd .
# docker run -t -dp 3000:3000 mvd
