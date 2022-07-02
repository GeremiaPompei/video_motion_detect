# RUN AND COMPILE LOCALLY
rm -rf build
mkdir build
g++ src/main.cpp -o build/main `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -std=c++11
./build/main ./media/videoplayback.mp4 0.20

# RUN AND COMPILE DOCKER
# docker build -t mvd .
# docker run -t -dp 3000:3000 mvd