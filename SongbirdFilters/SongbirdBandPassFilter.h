/*
  ==============================================================================

    SongbirdBandPassFilter.h
    Created: 16 Jul 2016 5:40:26pm
    Author:  Jack Devlin

  ==============================================================================
*/

#ifndef SONGBIRDBANDPASSFILTER_H_INCLUDED
#define SONGBIRDBANDPASSFILTER_H_INCLUDED

#include "ParameterData.h"
#include "DspFilters/Legendre.h"

// an individual band pass filter, with built in gain
class SongbirdBandPassFilter {
public:
    
    SongbirdBandPassFilter() : lowPass(), highPass(), gainAbs(1) {}
        
    void setup(double newSampleRate, double newFrequency, double newBandWidth, int gaindB) {
        
        sampleRate = newSampleRate;
        frequency = newFrequency;
        bandWidth = newBandWidth;
        
        lowPass.setup(FILTER_ORDER, sampleRate, frequency + bandWidth / 2);
        highPass.setup(FILTER_ORDER, sampleRate, frequency - bandWidth / 2);
        
        
        gainAbs = pow(10, gaindB / 20.0);
    }
    
    void process(float* inSamples, int numSamples) {
        float** samplesPtrPtr {&inSamples};
        
        lowPass.process(numSamples, samplesPtrPtr);
        highPass.process(numSamples, samplesPtrPtr);
        
        // apply gain reduction
        std::transform(inSamples, &inSamples[numSamples-1], inSamples, std::bind1st(std::multiplies<double>(), gainAbs));
    }
    
    void reset() {
        lowPass.reset();
        highPass.reset();
    }
    
    std::string dump() {
        std::string output;
        
        output.append("gainAbs: ");
        output.append(std::to_string(gainAbs));
        output.append(" frequency: ");
        output.append(std::to_string(frequency));
        output.append(" bandWidth: ");
        output.append(std::to_string(bandWidth));
        output.append(" sampleRate: ");
        output.append(std::to_string(sampleRate));
        
        return output;
    }
    
private:
    Dsp::SimpleFilter<Dsp::Legendre::LowPass<FILTER_ORDER>, 1> lowPass;
    Dsp::SimpleFilter<Dsp::Legendre::HighPass<FILTER_ORDER>, 1> highPass;
    
    
    double gainAbs;
    
    // store these to allow easy debugging output
    double  sampleRate,
            frequency,
            bandWidth;
    
};





#endif  // SONGBIRDBANDPASSFILTER_H_INCLUDED
