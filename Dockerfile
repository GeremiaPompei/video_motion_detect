### start from current ubuntu image
FROM ubuntu
### install base programming tools
RUN apt update && DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt -y install tzdata \
    && apt install -y git subversion \
    vim cmake autoconf libreadline-dev \
    software-properties-common \
    hwloc emacs
### make g++-10 default g++ compiler
RUN apt -y install g++-10
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 80 --slave /usr/bin/g++ g++ /usr/bin/g++-10 --slave /usr/bin/gcov gcov /usr/bin/gcov-10
### install FastFlow
RUN cd /usr/local/ && git clone https://github.com/fastflow/fastflow.git
RUN cd /usr/local/include && ln -s ../fastflow/ff . 
RUN cd /usr/local/fastflow/ff && yes | ./mapping_string.sh 
### install GRPPi
RUN cd /usr/local && git clone https://github.com/arcosuc3m/grppi.git
RUN cd /usr/local/include && ln -s ../grppi/include/grppi 
### install rplsh
RUN cd /usr/local && git clone https://github.com/t-costa/rplsh.git 
RUN ln -s /usr/local/rplsh/build/rplsh /usr/local/bin/rplsh
### https://github.com/Murray1991/rplsh.git
RUN cd /usr/local/rplsh && ./install.sh
### set up service ssh
RUN apt install -y ssh 
RUN service ssh start
### add additional packages
RUN apt install -y libtbb-dev \
	rsync
RUN apt install -y  simplescreenrecorder
RUN apt install -y  openconnect
RUN apt install -y  network-manager-openconnect
RUN apt install -y  libopencv-dev
### RUN apt install -y  xterm x11-apps
### update compiler
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt update 
RUN apt install apt-utils
RUN apt install -y g++-11
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11
### install htop
RUN apt install -y htop
### fix user
RUN useradd -ms /bin/bash spm2021user
WORKDIR /home/spm2021user
USER spm2021user

# COPY --chown=spm2021user . .
# RUN ./build.sh
# RUN ./test.sh