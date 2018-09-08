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

#pragma once

// DSPFilters sets off a lot of clang warnings - disable them for Butterworth.h only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#pragma clang diagnostic ignored "-Wold-style-cast"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#endif

#include "DspFilters/Butterworth.h"

#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

/**
 * A simple bandpass filter which can process mono or stereo signals.
 * Was initially created to remove frequencies at the extremes of the human
 * hearing range to clean up audio but can fulfil any typical bandpass
 * filter purpose.
 *
 * The cutoff frequencies cannot be changed once the object is constructed. 
 *
 * Has methods for processing either a mono or stereo buffer of samples.
 *
 * @see setSampleRate   - recommended to call before performing any processing
 */
namespace WECore::Carve {

    class NoiseFilter {
    public:
        
        /**
         * Defaults the sample rate. It is recommended to call setSampleRate manually
         * before attempting any processing.
         *
         * @param   lowCutHz    Everything below this frequency will be cut
         * @param   highCutHz   Everything above this frequency will be cut
         */
        NoiseFilter(float lowCutHz, float highCutHz) : _lowCutHz(lowCutHz),
                                                       _highCutHz(highCutHz) {
            setSampleRate(44100);
        }
        
        virtual ~NoiseFilter() {}
        
        /**
         * Configures the filters for the correct sample rate. Ensure this is
         * called before attempting to process audio.
         *
         * @param   sampleRate  The sample rate the filter should be configured for
         */
        inline void setSampleRate(double sampleRate);
        
        /**
         * Resets all filters.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        inline void reset();
        
        /**
         * Applies the filtering to a mono buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param   inSample    Pointer to the first sample of the buffer
         * @param   numSamples  Number of samples in the buffer
         */
        inline void Process1in1out(float* inSample, int numSamples);
        
        /**
         * Applies the filtering to a stereo buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param   inLeftSample    Pointer to the first sample of the left channel's buffer
         * @param   inRightSample   Pointer to the first sample of the right channel's buffer
         * @param   numSamples      Number of samples in the buffer. The left and right buffers
         *                          must be the same size.
         */
        inline void Process2in2out(float *inLeftSample, float *inRightSample, int numSamples);
        
    private:
        static constexpr int FILTER_ORDER {4};
        Dsp::SimpleFilter<Dsp::Butterworth::LowPass<FILTER_ORDER>, 1> _monoHighCutFilter;
        Dsp::SimpleFilter<Dsp::Butterworth::LowPass<FILTER_ORDER>, 2> _stereoHighCutFilter;
        
        Dsp::SimpleFilter<Dsp::Butterworth::HighPass<FILTER_ORDER>, 1> _monoLowCutFilter;
        Dsp::SimpleFilter<Dsp::Butterworth::HighPass<FILTER_ORDER>, 2> _stereoLowCutFilter;
        
        float   _lowCutHz,
                _highCutHz;
    };

    void NoiseFilter::setSampleRate(double sampleRate) {
        _monoLowCutFilter.setup(FILTER_ORDER, sampleRate, _lowCutHz);
        _stereoLowCutFilter.setup(FILTER_ORDER, sampleRate, _lowCutHz);
        _monoHighCutFilter.setup(FILTER_ORDER, sampleRate, _highCutHz);
        _stereoHighCutFilter.setup(FILTER_ORDER, sampleRate, _highCutHz);
    }

    void NoiseFilter::reset() {
        _monoLowCutFilter.reset();
        _monoHighCutFilter.reset();
        _stereoLowCutFilter.reset();
        _stereoHighCutFilter.reset();
    }

    void NoiseFilter::Process1in1out(float* inSample, int numSamples) {
        _monoLowCutFilter.process(numSamples, &inSample);
        _monoHighCutFilter.process(numSamples, &inSample);
    }

    void NoiseFilter::Process2in2out(float *inLeftSample, float *inRightSample, int numSamples) {
        float** channelsArray = new float*[2];
        channelsArray[0] = inLeftSample;
        channelsArray[1] = inRightSample;
        _stereoLowCutFilter.process(numSamples, channelsArray);
        _stereoHighCutFilter.process(numSamples, channelsArray);
        delete [] channelsArray;
    }
}
