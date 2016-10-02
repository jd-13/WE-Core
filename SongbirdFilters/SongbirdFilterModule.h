/*
  ==============================================================================

    SongbirdFilterModule.h
    Created: 12 Jun 2016 7:25:17pm
    Author:  Jack Devlin

  ==============================================================================
*/

#ifndef SONGBIRDFILTER_H_INCLUDED
#define SONGBIRDFILTER_H_INCLUDED

#include "ParameterData.h"
#include "SongbirdFormantFilter.h"
#include <map>

static const int NUM_FORMANTS_PER_VOWEL {5};
static const int NUM_VOWELS {5};

enum class Channels {
    LEFT,
    RIGHT
};

// a set of 5 pairs of filters for vowel 1, and another set for vowel 2
class SongbirdFilterModule {
public:
    SongbirdFilterModule() :    vowel1(VOWEL_A),
                                vowel2(VOWEL_E),
                                filterPosition(FILTER_POSITION_DEFAULT),
                                sampleRate(44100),
                                mix(MIX_DEFAULT) {
        
        // initialise the filters to some default values
        setVowel1(vowel1);
        setVowel2(vowel2);
    }
    
    void setVowel1(int val) {
        // perform a bounds check, then apply the appropriate formants
        vowel1 = boundsCheck(val, VOWEL_MIN, VOWEL_MAX);
        
        const std::vector<Formant> tempFormants(&allFormants[vowel1 - 1][0],
                                                &allFormants[vowel1 - 1][NUM_FORMANTS_PER_VOWEL]);
        
        filters1[Channels::LEFT].setFormants(tempFormants, sampleRate);
        filters1[Channels::RIGHT].setFormants(tempFormants, sampleRate);
    }
    
    void setVowel2(int val) {
        // perform a bounds check, then apply the appropriate formants
        vowel2 = boundsCheck(val, VOWEL_MIN, VOWEL_MAX);
        
        const std::vector<Formant> tempFormants(&allFormants[vowel2 - 1][0],
                                                &allFormants[vowel2 - 1][NUM_FORMANTS_PER_VOWEL]);
        filters2[Channels::LEFT].setFormants(tempFormants, sampleRate);
        filters2[Channels::RIGHT].setFormants(tempFormants, sampleRate);
    }
    
    void setFilterPosition(float val) {
        filterPosition = boundsCheck(val, FILTER_POSITION_MIN, FILTER_POSITION_MAX);
    }
    
    void setSampleRate(float val) {
        sampleRate = boundsCheck(val, static_cast<float>(44100), static_cast<float>(192000));
        
        setVowel1(vowel1);
        setVowel2(vowel2);
    }
    
    void setMix(float val) { mix = boundsCheck(val, MIX_MIN, MIX_MAX); }
    
    void reset() {
        filters1[Channels::LEFT].reset();
        filters1[Channels::RIGHT].reset();
        filters2[Channels::LEFT].reset();
        filters2[Channels::RIGHT].reset();
    }
    
    int getVowel1() { return vowel1; }
    
    int getVowel2() { return vowel2; }
    
    float getFilterPosition() { return filterPosition; }
    
    float getMix() { return mix; }
    
    void Process2in2out(float* leftSamples,
                        float* rightSamples,
                        int numSamples) {
        
        if (leftSamples != nullptr && rightSamples != nullptr && numSamples > 0) {
            // create two buffers of dry samples
            std::map<Channels, std::vector<float>> outputBuffer1;
            outputBuffer1[Channels::LEFT] = std::vector<float>(leftSamples,
                                                               leftSamples + numSamples);
            outputBuffer1[Channels::RIGHT] = std::vector<float>(rightSamples,
                                                                rightSamples + numSamples);
            
            std::map<Channels, std::vector<float>> outputBuffer2(outputBuffer1);
            
            // do the processing for each filter
            filters1[Channels::LEFT].process(&outputBuffer1[Channels::LEFT][0], numSamples);
            filters1[Channels::RIGHT].process(&outputBuffer1[Channels::RIGHT][0], numSamples);
            
            filters2[Channels::LEFT].process(&outputBuffer2[Channels::LEFT][0], numSamples);
            filters2[Channels::RIGHT].process(&outputBuffer2[Channels::RIGHT][0], numSamples);
            
            // write to output, applying filter position and mix level
            for (size_t iii {0}; iii < numSamples; iii++) {
                leftSamples[iii] =  leftSamples[iii] * (1 - mix)
                + outputBuffer1[Channels::LEFT][iii] * (1 - filterPosition) * mix
                + outputBuffer2[Channels::LEFT][iii] * filterPosition * mix;
                
                rightSamples[iii] = rightSamples[iii] * (1 - mix)
                + outputBuffer1[Channels::RIGHT][iii] * (1 - filterPosition) * mix
                + outputBuffer2[Channels::RIGHT][iii] * filterPosition * mix;
            }
        }
    }
    
private:
    template<typename T>
    T boundsCheck(T param, T min, T max) {
        if (param < min) param = min;
        if (param > max) param = max;
        
        return param;
    }
    
    int vowel1;
    int vowel2;
    float filterPosition;
    float sampleRate;
    float mix;
    
    std::map<Channels, SongbirdFormantFilter> filters1;
    std::map<Channels, SongbirdFormantFilter> filters2;
    
    // an array of all the formants that will be needed (TODO: could be made static again)
    const Formant allFormants[NUM_VOWELS][NUM_FORMANTS_PER_VOWEL] {
        {Formant(800, 0, 80), Formant(1150, -4, 90), Formant(2800, -20, 120), Formant(3500, -36, 130), Formant(4950, -60, 140)},
        {Formant(400, 0, 60), Formant(1600, -24, 80), Formant(2700, -30, 120), Formant(3300, -35, 150), Formant(4950, -60, 200)},
        {Formant(350, 0, 50), Formant(1700, -20, 100), Formant(2700, -30, 120), Formant(3700, -36, 150), Formant(4950, -60, 200)},
        {Formant(450, 0, 70), Formant(800, -9, 80), Formant(2830, -16, 100), Formant(3500, -28, 130), Formant(4950, -55, 135)},
        {Formant(325, 0, 50), Formant(700, -12, 60), Formant(2530, -30, 170), Formant(3500, -40, 180), Formant(4950, -64, 200)},
    };
};

#endif  // SONGBIRDFILTER_H_INCLUDED
