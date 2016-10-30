/*
 *	File:		CarveNoiseFilter.h
 *
 *	Version:	2.0.0
 *
 *	Created:	02/06/2016
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
 *
 */

#ifndef CARVENOISEFILTER_H_INCLUDED
#define CARVENOISEFILTER_H_INCLUDED

#include "DspFilters/Butterworth.h"

/**
 * A simple bandpass filter which can process mono or stereo signals.
 * Was initially created to remove frequencies at the extremes of the human
 * hearing range to clean up audio but can fulfil any typical bandpass
 * filter purpose.
 *
 * Has methods for processing either a mono or stereo buffer of samples.
 *
 * @see setSampleRate   - recommended to call before performing any processing
 */
class CarveNoiseFilter {
public:
    
    /**
     * Defaults the sample rate. It is recommended to call setSampleRate manually
     * before attempting any processing.
     *
     * @param   lowCutHz    Everything below this frequency will be cut
     * @param   highCutHz   Everything above this frequency will be cut
     */
    CarveNoiseFilter(float lowCutHz,
                     float highCutHz) : _lowCutHz(lowCutHz),
                                        _highCutHz(highCutHz) {
        setSampleRate(44100);
    }
    
    virtual ~CarveNoiseFilter() {}
    
    /**
     * Configures the filters for the correct sample rate. Ensure this is
     * called before attempting to process audio.
     *
     * @param   sampleRate  The sample rate the filter should be configured for
     */
    void setSampleRate(double sampleRate) {
        monoLowCutFilter.setup(FILTER_ORDER, sampleRate, _lowCutHz);
        stereoLowCutFilter.setup(FILTER_ORDER, sampleRate, _lowCutHz);
        monoHighCutFilter.setup(FILTER_ORDER, sampleRate, _highCutHz);
        stereoHighCutFilter.setup(FILTER_ORDER, sampleRate, _highCutHz);
    }
    
    /**
     * Resets all filters.
     * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
     */
    void reset() {
        monoLowCutFilter.reset();
        monoHighCutFilter.reset();
        stereoLowCutFilter.reset();
        stereoHighCutFilter.reset();
    }
    
    /**
     * Applies the filtering to a mono buffer of samples.
     * Expect seg faults or other memory issues if arguements passed are incorrect.
     *
     * @param   inSample    Pointer to the first sample of the buffer
     * @param   numSamples  Number of samples in the buffer
     */
    void ApplyMonoFiltering(float* inSample, int numSamples) {
        monoLowCutFilter.process(numSamples, &inSample);
        monoHighCutFilter.process(numSamples, &inSample);
    }
    
    /**
     * Applies the filtering to a stereo buffer of samples.
     * Expect seg faults or other memory issues if arguements passed are incorrect.
     *
     * @param   inLeftSample    Pointer to the first sample of the left channel's buffer
     * @param   inRightSample   Pointer to the first sample of the right channel's buffer
     * @param   numSamples      Number of samples in the buffer. The left and right buffers
     *                          must be the same size.
     */
    void ApplyStereoFiltering(float *inLeftSample, float *inRightSample, int numSamples) {
        float** channelsArray = new float*[2];
        channelsArray[0] = inLeftSample;
        channelsArray[1] = inRightSample;
        stereoLowCutFilter.process(numSamples, channelsArray);
        stereoHighCutFilter.process(numSamples, channelsArray);
        delete [] channelsArray;
    }
    
private:
    static const int FILTER_ORDER {4};
    Dsp::SimpleFilter<Dsp::Butterworth::LowPass<4>, 1> monoHighCutFilter;
    Dsp::SimpleFilter<Dsp::Butterworth::LowPass<4>, 2> stereoHighCutFilter;
    
    Dsp::SimpleFilter<Dsp::Butterworth::HighPass<4>, 1> monoLowCutFilter;
    Dsp::SimpleFilter<Dsp::Butterworth::HighPass<4>, 2> stereoLowCutFilter;
    
    float   _lowCutHz,
            _highCutHz;
};



#endif  // CARVENOISEFILTER_H_INCLUDED
