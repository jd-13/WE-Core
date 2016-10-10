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

/**
 * An individual band pass filter, designed to create a single formant peak.
 * Contains a pair of butterworth filters and an adjustable gain control.
 *
 * Supports only mono audio processing. For stereo processing, you must create
 * two objects of this type. (Do not reuse this object for both channels)
 *
 * Compensation is applied to account for any reduction in amplitude of the
 * frequencies in the pass band, such that the amplitude of the centre frequency
 * will not be significantly changed.
 *
 * @see setup   - must be called before performing any processing
 */
class SongbirdBandPassFilter {
public:
    
    /**
     * Constructs internal filters but does not prepare them for processing.
     *
     * @see setup - must be called before performing any processing
     */
    SongbirdBandPassFilter() : lowPass(), highPass(), gainAbs(1) {}
    
    /**
     * Prepares the filters for processing. Call this before performing any processing, or
     * whenever any of the parameters need updating.
     *
     * @param   newSampleRate   Sample rate which should be used for processing future samples
     * @param   newFrequency    Centre frequency of the filter
     * @param   newBandWidth    Bandwidth of the filter
     * @param   gaindB          Gain value in decibels to be applied to the signal after processing
     */
    void setup(double newSampleRate, double newFrequency, double newBandWidth, int gaindB) {
        
        sampleRate = newSampleRate;
        frequency = newFrequency;
        bandWidth = newBandWidth;
        
        lowPass.setup(FILTER_ORDER, sampleRate, frequency + bandWidth / 2);
        highPass.setup(FILTER_ORDER, sampleRate, frequency - bandWidth / 2);
        
        
        gainAbs = pow(10, gaindB / 20.0);
    }
    
    /**
     * Applies the filtering to a buffer of samples.
     * Expect seg faults or other memory issues if arguements passed are incorrect.
     *
     * @param   inSample    Pointer to the first sample of the buffer
     * @param   numSamples  Number of samples in the buffer
     */
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
    
    /**
     * Resets all filters.
     * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
     */
    void reset() {
        lowPass.reset();
        highPass.reset();
    }
    
    /**
     * Dump the current values of the filter's parameters to a string in an
     * easy to read format.
     * (Useful for debugging)
     *
     * @return  A std::string which contains the parameters of the filter
     */
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
    
    /**
     * Value to multiply the output by to correct for the loss in amplitude
     */
    const double gainCompensation {2};
    
    double gainAbs;
    
    //@{
    /**
     * Stored to allow easy debugging output
     */
    double  sampleRate,
            frequency,
            bandWidth;
    //@}
    
};





#endif  // SONGBIRDBANDPASSFILTER_H_INCLUDED
