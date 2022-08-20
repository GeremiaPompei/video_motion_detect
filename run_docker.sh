# RUN AND COMPILE DOCKER
docker build -t mvd . 
docker run --volume=$(pwd):/home/spm2021user -it mvd bash