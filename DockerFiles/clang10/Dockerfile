FROM ubuntu:focal

# Envs
ENV PATH /opt/llvm/bin:$PATH
ENV CXX clang++-10.0
ENV CATCH_PATH /home/catch/
ENV VALGRIND_PATH /home/valgrind

# Prevents "Configuring tzdata" dialog from stalling build
ENV DEBIAN_FRONTEND=noninteractive

CMD bash

# Basics
RUN apt-get update
RUN apt-get install -y apt-utils libasound2-dev webkit2gtk-4.0 x11proto-xinerama-dev
RUN apt-get install -y wget unzip git subversion curl libcurl4-gnutls-dev cppcheck cmake

# Catch
RUN wget https://github.com/catchorg/Catch2/releases/download/v2.13.0/catch.hpp -P "/home/catch/"

# Clang
RUN apt-get install -y software-properties-common
RUN wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN add-apt-repository -y "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-10 main"
RUN apt-get update
RUN apt-get install -y clang-10

# Valgrind
RUN apt-get install -y autotools-dev automake
RUN svn co svn://svn.valgrind.org/valgrind/trunk /home/valgrind && cd /home/valgrind && ./autogen.sh && ./configure && make && make install

# VST SDK
RUN wget http://www.steinberg.net/sdk_downloads/vstsdk367_03_03_2017_build_352.zip
RUN mkdir ~/SDKs && unzip vstsdk367_03_03_2017_build_352.zip -d ~/SDKs/
