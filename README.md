[![Build Status](https://travis-ci.org/jd-13/WE-Core.svg?branch=master)](https://travis-ci.org/jd-13/WE-Core)
[![codecov](https://codecov.io/gh/jd-13/WE-Core/branch/master/graph/badge.svg)](https://codecov.io/gh/jd-13/WE-Core)

# WE-Core
A set of core libraries for useful DSP related classes that are used by multiple White Elephant
Audio VSTs and Audio Units.  

This is a headers only library, to use the DSP classes in your own projects add the include path:
`<your WE-Core directory>/WECore`.  

## DSP Classes
The naming convention is that each class is prefixed with the product it was developed for.  

### LFOs:  
RichterLFO and RichterMOD - substantial functionality with tempo sync, and multiple wave types and
parameters  

### Filters:  
CarveNoiseFilter - a simple filter to remove noise at the extremes of human hearing  
SongbirdFormantFilter - Contains multiple SongbirdBandPassFilters, designed to create vowel sounds  
SongbirdFilterModule - Contains two SongbirdFormantFilters which can be blended between, with
multiple supported vowel sounds built in  
TPTSVFilter - Topology preserving filter, configurable as high pass, low pass, or peak  

### Distortions:  
CarveDSPUnit - A waveshaping distortion module with multiple wave shapes, pre and post gain control,
and a "tweak" control which morphs the selected wave shape  

### Stereo Processing:  
MONSTRCrossover - A crossover filter made of several MONSTRBand units. Uses the MONSTRBand units to
provide multiband stereo width control 

## Documentation  
Documentation is available at: https://www.whiteelephantaudio.com/doxygen/WECore/html/  

## Required Libraries  
Some classes within this library require:  

A Collection of Useful C++ Classes for Digital Signal Processing: https://github.com/vinniefalco/DSPFilters

LookAndFeel classes are for building UIs using the JUCE library: https://www.juce.com/  

cURL (for the experimental auto update functionality): https://curl.haxx.se/libcurl/ 

## Builds and Testing
Each DSP module has its own set of tests, found under the `WECore` directory. The file
`CMakeLists.txt` can be used to create executable binarys which run the tests. This is done as
follows:  

    export WECORE_HOME=<path to your WECore>
    export WECORE_SRC=$WECORE_SRC/WECore
    cd $WECORE_HOME
    mkdir build
    cd build
    cmake ..
    make

This produces the binaries:
* `WECoreTest` - Contains the standard set of unit tests
* `WECoreTestPerf` - Contains unit tests for measuring performance
