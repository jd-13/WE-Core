/*
 *  File:       SongbirdBandPassFilter.h
 *
 *  Version:    1.0.0
 *
 *  Created:    16/07/2016
 *
 *	This file is part of WECore.
 *
 *  WECore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WECore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WECore.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SONGBIRDBANDPASSFILTER_H_INCLUDED
#define SONGBIRDBANDPASSFILTER_H_INCLUDED

#include "DspFilters/Butterworth.h"
#include "SongbirdFiltersParameters.h"

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
        std::transform(inSamples,
                       &inSamples[numSamples-1],
                       inSamples,
                       std::bind1st(std::multiplies<double>(), gainAbs * gainCompensation));
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
    Dsp::SimpleFilter<Dsp::Butterworth::LowPass<FILTER_ORDER>, 1> lowPass;
    Dsp::SimpleFilter<Dsp::Butterworth::HighPass<FILTER_ORDER>, 1> highPass;
    
    // value to multiply the output by to correct for the loss in amplitude
    const double gainCompensation {2};
    
    double gainAbs;
    
    // store these to allow easy debugging output
    double  sampleRate,
            frequency,
            bandWidth;
    
};





#endif  // SONGBIRDBANDPASSFILTER_H_INCLUDED
