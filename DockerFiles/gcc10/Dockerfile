FROM ubuntu:focal

# Prevents "Configuring tzdata" dialog from stalling build
ENV DEBIAN_FRONTEND=noninteractive

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
RUN apt-get install -y g++-10

ENV CC /usr/bin/gcc-10
ENV CXX /usr/bin/g++-10
ENV COVERAGE_COMMAND /usr/bin/gcov-10
ENV COVERAGE_FLAGS --coverage

# Valgrind
RUN apt-get install -y valgrind

# VST SDK
RUN wget https://www.steinberg.net/vst3sdk
RUN unzip vst3sdk -d /home

# JUCE SDK
ARG JUCE_VERSION=6.1.6
RUN wget https://github.com/juce-framework/JUCE/releases/download/${JUCE_VERSION}/juce-${JUCE_VERSION}-linux.zip
RUN unzip juce-${JUCE_VERSION}-linux.zip -d /home
ENV JUCE_PATH /home/JUCE

# Pluginval
RUN wget https://github.com/Tracktion/pluginval/releases/download/v0.2.9/pluginval_Linux.zip
RUN unzip pluginval_Linux.zip -d /home
ENV PLUGINVAL /home/pluginval
