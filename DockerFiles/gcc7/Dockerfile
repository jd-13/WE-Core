FROM ubuntu:focal

# Envs
ENV CC /usr/bin/gcc-7
ENV CXX g++-7

# Prevents "Configuring tzdata" dialog from stalling build
ENV DEBIAN_FRONTEND=noninteractive

ENV COVERAGE_COMMAND /usr/bin/gcov-7
ENV COVERAGE_FLAGS --coverage

CMD bash

# Basics
RUN apt-get update
RUN apt-get install -y apt-utils libasound2-dev webkit2gtk-4.0 x11proto-xinerama-dev
RUN apt-get install -y wget unzip git subversion curl libcurl4-gnutls-dev cppcheck cmake

# Catch
RUN wget https://github.com/catchorg/Catch2/releases/download/v2.13.0/catch.hpp -P "/home/catch/"
ENV CATCH_PATH /home/catch/

# GCC
RUN apt-get install -y software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update
RUN apt-get install -y g++-7

# Valgrind
RUN apt-get install -y autotools-dev automake
RUN wget https://sourceware.org/pub/valgrind/valgrind-3.16.1.tar.bz2 -P "/home"
RUN mkdir -p /home/valgrind && tar -xf /home/valgrind-3.16.1.tar.bz2 -C /home/valgrind
RUN mv /home/valgrind/valgrind-3.16.1/* /home/valgrind && rm -rf /home/valgrind/valgrind-3.16.1
RUN cd /home/valgrind && ./autogen.sh && ./configure && make && make install
ENV VALGRIND_PATH /home/valgrind

# VST SDK
RUN wget http://www.steinberg.net/sdk_downloads/vstsdk367_03_03_2017_build_352.zip
RUN unzip vstsdk367_03_03_2017_build_352.zip -d /home

# JUCE SDK
RUN wget https://github.com/juce-framework/JUCE/releases/download/6.0.1/juce-6.0.1-linux.zip
RUN unzip juce-6.0.1-linux.zip -d /home
ENV JUCE_PATH /home/JUCE

# Pluginval
RUN wget https://github.com/Tracktion/pluginval/releases/download/v0.2.9/pluginval_Linux.zip
RUN unzip pluginval_Linux.zip -d /home
ENV PLUGINVAL /home/pluginval
