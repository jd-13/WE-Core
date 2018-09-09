FROM ubuntu:bionic

# Envs
ENV CC /usr/bin/gcc-7
ENV CXX g++-7
ENV CATCH_PATH /home/catch/
ENV VALGRIND_PATH /home/valgrind

ENV COVERAGE_COMMAND /usr/bin/gcov-7
ENV COVERAGE_FLAGS --coverage

CMD bash

# Basics
RUN apt-get update
RUN apt-get install -y apt-utils libasound2-dev webkit2gtk-4.0 x11proto-xinerama-dev
RUN apt-get install -y wget unzip git subversion curl libcurl4-gnutls-dev cppcheck cmake

# Catch
COPY catch.hpp "/home/catch/catch.hpp"

# GCC
RUN apt-get install -y software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update
RUN apt-get install -y g++-7

# Valgrind
RUN svn co svn://svn.valgrind.org/valgrind/trunk /home/valgrind && cd /home/valgrind && ./autogen.sh && ./configure && make && make install

# VST SDK
RUN wget http://www.steinberg.net/sdk_downloads/vstsdk367_03_03_2017_build_352.zip
RUN mkdir ~/SDKs && unzip vstsdk367_03_03_2017_build_352.zip -d ~/SDKs/
