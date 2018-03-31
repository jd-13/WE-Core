/*
 *	File:		MONSTRBand.h
 *
 *	Version:	1.0.0
 *
 *	Created:	21/01/2016
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

#ifndef MONSTRBAND_H_INCLUDED
#define MONSTRBAND_H_INCLUDED

#include "MONSTRParameters.h"
#include "General/CoreMath.h"


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


// forward declaration to allow friend class
class MONSTRCrossover;

/**
 * Provides stereo width control of a single frequency range.
 *
 * It is recommended to use this class through the MONSTRCrossover class.
 *
 * This class contains a highpass and a lowpass filter in series (yes
 * this is a roundabout way of creating a bandpass but this appeared to
 * perform better), the passband can then have its stereo width increased
 * or reduced.
 *
 * Internally relies on the parameters provided in MONSTRParameters.h
 *
 * @see MONSTRCrossover
 */
class MONSTRBand {
public:
    
    /**
     * Performs setup of the filters using default settings. The arguments are
     * used to tell the MONSTRBand object which part of the frequency spectrum
     * it will be covering as crossover filter.
     *
     * NOTE: The parameters cannot both be true. This will cause undefined behaviour.
     *
     * @param   newIsLower  If true, this tells the MONSTRBand object it is covering
     *                      the lowest freqency range in the crossover. This will deactivate
     *                      its highpass (low cut) filter.
     * @param   newIsUpper  If true, this tells the MONSTRBand object it is covering
     *                      the highest freqency range in the crossover. This will deactivate
     *                      its lowpass (high cut) filter.
     *
     */
    MONSTRBand(bool newIsLower, bool newIsUpper) :  isActive(BANDSWITCH_DEFAULT),
                                                    isLower(newIsLower),
                                                    isUpper(newIsUpper),
                                                    width(WIDTH.defaultValue),
                                                    lowCutoffHz(CROSSOVERLOWER.defaultValue),
                                                    highCutoffHz(CROSSOVERUPPER.defaultValue),
                                                    sampleRate(44100),
                                                    lowCut1(),
                                                    lowCut2(),
                                                    highCut1(),
                                                    highCut2() {
        lowCut1.setup(FILTER_ORDER, sampleRate, lowCutoffHz);
        lowCut2.setup(FILTER_ORDER, sampleRate, lowCutoffHz);
        highCut1.setup(FILTER_ORDER, sampleRate, highCutoffHz);
        highCut2.setup(FILTER_ORDER, sampleRate, highCutoffHz);
    }

    virtual ~MONSTRBand() {}
    
    friend class MONSTRCrossover;
    
    /**
     * Sets the stereo width of this band.
     * Higher value = more width
     *
     * @param   val   The frequency in Hz to set the width to
     *
     * @see     WIDTH for valid values
     */
    void setWidth(float val) { width = WIDTH.BoundsCheck(val); }

    /**
     * Sets whether this band will modify the stereo width of the signal it receives or not.
     *
     * If the band is deactivated filtering will still be performed. This is so that a 
     * crossover containing multiple MONSTRBands will always be able easily to sum the
     * output of the bands to unity gain, whether some of the MONSTRBands are bypassed
     * or not.
     *
     * @param   val If true, stereo width processing will be applied
     */
    void setIsActive(bool val) { isActive = val; }

    /**
     * @see setWidth
     */
    float getWidth() const { return width; }

    /**
     * @see setIsActive
     */
    bool getIsActive() const { return isActive; }

private:
    bool    isActive,
            isLower,
            isUpper;

    float   width,
            lowCutoffHz,
            highCutoffHz;

    double sampleRate;
    static const int FILTER_ORDER {2};

    Dsp::SimpleFilter<Dsp::Butterworth::HighPass<2>, 2> lowCut1;
    Dsp::SimpleFilter<Dsp::Butterworth::HighPass<2>, 2> lowCut2;
    Dsp::SimpleFilter<Dsp::Butterworth::LowPass<2>, 2> highCut1;
    Dsp::SimpleFilter<Dsp::Butterworth::LowPass<2>, 2> highCut2;


    void filterSamples(double *inLeftSamples, double *inRightSamples, int numSamples) {
        double** channelsArray = new double*[2];
        channelsArray[0] = inLeftSamples;
        channelsArray[1] = inRightSamples;

        if (isLower) {
            highCut1.process(numSamples, channelsArray);
            highCut2.process(numSamples, channelsArray);
        } else if (isUpper) {
            lowCut1.process(numSamples, channelsArray);
            lowCut2.process(numSamples, channelsArray);
        } else {
            lowCut1.process(numSamples, channelsArray);
            lowCut2.process(numSamples, channelsArray);
            highCut1.process(numSamples, channelsArray);
            highCut2.process(numSamples, channelsArray);
        }

        delete[] channelsArray;
    }
    
    /**
     * Lets the band know if it covers the lowest frequencies, so will
     * apply only a high cut filter.
     */
    void makeBandLower() {
        isLower = true;
        isUpper = false;
    }
    
    /**
     * Lets the band know if it covers the middle frequencies, so will
     * apply both a low and high cut filter.
     */
    void makeBandMiddle() {
        isLower = false;
        isUpper = false;
    }
    
    /**
     * Lets the band know if it covers the highest frequencies, so will
     * apply only a low cut filter.
     */
    void makeBandUpper() {
        isLower = false;
        isUpper = true;
    }
    
    /**
     * Resets filter states. Call before beginning a new buffer of
     * samples.
     */
    void reset() {
        lowCut1.reset();
        lowCut2.reset();
        highCut1.reset();
        highCut2.reset();
    }
    
    void setSampleRate(double newSampleRate) {
        // if the new sample rate is different, recalculate the filter coefficients
        if (!CoreMath::compareFloatsEqual(newSampleRate, sampleRate)) {
            sampleRate = newSampleRate;
            setLowCutoff(lowCutoffHz);
            setHighCutoff(highCutoffHz);
        }
    }
    
    void setLowCutoff(float val) {
        // if this is the lowest band, then do not cut the low frequencies
        if (!isLower && !isUpper) {
            lowCutoffHz = CROSSOVERLOWER.BoundsCheck(val);
            lowCut1.setup(FILTER_ORDER, sampleRate, lowCutoffHz);
            lowCut2.setup(FILTER_ORDER, sampleRate, lowCutoffHz);
        } else if (isUpper) {
            lowCutoffHz = CROSSOVERUPPER.BoundsCheck(val);
            lowCut1.setup(FILTER_ORDER, sampleRate, lowCutoffHz);
            lowCut2.setup(FILTER_ORDER, sampleRate, lowCutoffHz);
        }
    }
    
    void setHighCutoff(float val) {
        // if this is the highest band, then do not cut the high frequencies
        if (!isLower && !isUpper) {
            highCutoffHz = CROSSOVERUPPER.BoundsCheck(val);
            highCut1.setup(FILTER_ORDER, sampleRate, highCutoffHz);
            highCut2.setup(FILTER_ORDER, sampleRate, highCutoffHz);
        } else if (isLower) {
            highCutoffHz = CROSSOVERLOWER.BoundsCheck(val);
            highCut1.setup(FILTER_ORDER, sampleRate, highCutoffHz);
            highCut2.setup(FILTER_ORDER, sampleRate, highCutoffHz);
        }
    }
    
    float getLowCutoff() const { return lowCutoffHz; }
    
    float getHighCutoff() const { return highCutoffHz; }
    
    /**
     * Performs the effect processing on leftSample and rightSample. Use for
     * stereo in->stereo out signals.
     *
     * @param[out]   leftSample   Pointer to the first sample of the left channel's buffer
     * @param[out]   rightSample  Pointer to the first sample of the right channel's buffer
     * @param[in]    numSamples   Number of samples in the buffer. The left and right buffers
     *                            must be the same size.
     */
    void process2in2out(double* leftSample, double* rightSample, size_t numSamples) {
        
        // Apply the filtering before processing
        filterSamples(leftSample, rightSample, static_cast<int>(numSamples));
        
        if (isActive) {
            // Do the actual stereo widening or narrowing
            // Based on: http://musicdsp.org/showArchiveComment.php?ArchiveID=256
            double coef_S {width * 0.5};
            
            for (size_t iii {0}; iii < numSamples; iii++) {
                
                double mid {(leftSample[iii] + rightSample[iii]) * 0.5};
                double side {(rightSample[iii] - leftSample[iii]) * coef_S};
                
                leftSample[iii] = mid - side;
                rightSample[iii] = mid + side;
            }
        }
    }
};



#endif  // MONSTRBAND_H_INCLUDED
