# WE-Core
A set of core libraries for useful DSP related classes that are used by multiple White Elephant Audio VSTs and Audio Units.  

## Contains:  
### LFOs:  
RichterLFO and RichterMOD - substantial functionality with tempo sync, and multiple wave types and parameters  

### Filters:  
CarveNoiseFilter - a simple filter to remove noise at the extremes of human hearing  
SongbirdBandPassFilter - A sharp bandpass filter, designed to create a single formant peak  
SongbirdFormantFilter - Contains multiple SongbirdBandPassFilters, designed to create vowel sounds  
SongbirdFilterModule - Contains two SongbirdFormantFilters which can be blended between, with multiple supported vowel sounds built in  

### Distortions:  
CarveDSPUnit - A waveshaping distortion module with multiple wave shapes, pre and post gain control, and a "tweak" control which morphs the selected wave shape  

### Stereo Processing:  
MONSTRCrossover - A crossover filter made of several MONSTRBand units. Uses the MONSTRBand units to provide multiband stereo width control  


Note: The Songbird classes are not yet completed, and so may change frequently  

## Documentation  
Documentation is available at: https://www.whiteelephantaudio.com/doxygen/WECore/html/  
The documentation is generated from this github repo every 4 hours.

## Required Libraries  
Some classes within this library require:  
A Collection of Useful C++ Classes for Digital Signal Processing: https://github.com/vinniefalco/DSPFilters

LookAndFeel classes are for building UIs using the JUCE library:
https://www.juce.com/
