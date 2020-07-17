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

#pragma once

#include "MONSTRParameters.h"
#include "General/CoreMath.h"

// DSPFilters sets off a lot of clang warnings - disable them for Butterworth.h only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
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

namespace WECore::MONSTR {
    // forward declaration to allow friend class
    template <typename T>
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
    template <typename T>
    class MONSTRBand {
        static_assert(std::is_floating_point<T>::value,
                "Must be provided with a floating point template type");

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
        MONSTRBand(bool newIsLower, bool newIsUpper) :  _isActive(Parameters::BANDSWITCH_DEFAULT),
                                                        _isLower(newIsLower),
                                                        _isUpper(newIsUpper),
                                                        _width(Parameters::WIDTH.defaultValue),
                                                        _lowCutoffHz(Parameters::CROSSOVERLOWER.defaultValue),
                                                        _highCutoffHz(Parameters::CROSSOVERUPPER.defaultValue),
                                                        _sampleRate(44100),
                                                        _lowCut1(),
                                                        _lowCut2(),
                                                        _highCut1(),
                                                        _highCut2() {
            _lowCut1.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
            _lowCut2.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
            _highCut1.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
            _highCut2.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
        }

        virtual ~MONSTRBand() = default;

        /**
         * Sets the stereo width of this band.
         * Higher value = more width
         *
         * @param   val   The frequency in Hz to set the width to
         *
         * @see     WIDTH for valid values
         */
        void setWidth(double val) { _width = Parameters::WIDTH.BoundsCheck(val); }

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
        void setIsActive(bool val) { _isActive = val; }

        /**
         * @see setWidth
         */
        double getWidth() const { return _width; }

        /**
         * @see setIsActive
         */
        bool getIsActive() const { return _isActive; }

        /**
         * Lets the band know if it covers the lowest frequencies, so will apply only a high cut
         * filter.
         */
        inline void makeBandLower();

        /**
         * Lets the band know if it covers the middle frequencies, so will apply both a low and
         * high cut filter.
         */
        inline void makeBandMiddle();

        /**
         * Lets the band know if it covers the highest frequencies, so will apply only a low cut
         * filter.
         */
        inline void makeBandUpper();

        /**
         * Resets filter states. Call before beginning a new buffer of samples.
         */
        inline void reset();

        inline void setSampleRate(double newSampleRate);

        inline void setLowCutoff(double val);

        inline void setHighCutoff(double val);

        double getLowCutoff() const { return _lowCutoffHz; }

        double getHighCutoff() const { return _highCutoffHz; }

        /**
         * Performs the effect processing on leftSample and rightSample. Use for
         * stereo in->stereo out signals.
         *
         * @param[out]   leftSample   Pointer to the first sample of the left channel's buffer
         * @param[out]   rightSample  Pointer to the first sample of the right channel's buffer
         * @param[in]    numSamples   Number of samples in the buffer. The left and right buffers
         *                            must be the same size.
         */
        inline void process2in2out(T* leftSample, T* rightSample, size_t numSamples);

    private:
        bool    _isActive,
                _isLower,
                _isUpper;

        double   _width,
                 _lowCutoffHz,
                 _highCutoffHz;

        double _sampleRate;

        static constexpr int FILTER_ORDER {2};

        Dsp::SimpleFilter<Dsp::Butterworth::HighPass<FILTER_ORDER>, 2> _lowCut1;
        Dsp::SimpleFilter<Dsp::Butterworth::HighPass<FILTER_ORDER>, 2> _lowCut2;
        Dsp::SimpleFilter<Dsp::Butterworth::LowPass<FILTER_ORDER>, 2> _highCut1;
        Dsp::SimpleFilter<Dsp::Butterworth::LowPass<FILTER_ORDER>, 2> _highCut2;

        inline void _filterSamples(T* inLeftSamples, T* inRightSamples, int numSamples);
    };

    template <typename T>
    void MONSTRBand<T>::makeBandLower() {
        _isLower = true;
        _isUpper = false;
    }

    template <typename T>
    void MONSTRBand<T>::makeBandMiddle() {
        _isLower = false;
        _isUpper = false;
    }

    template <typename T>
    void MONSTRBand<T>::makeBandUpper() {
        _isLower = false;
        _isUpper = true;
    }

    template <typename T>
    void MONSTRBand<T>::reset() {
        _lowCut1.reset();
        _lowCut2.reset();
        _highCut1.reset();
        _highCut2.reset();
    }

    template <typename T>
    void MONSTRBand<T>::setSampleRate(double newSampleRate) {
        // if the new sample rate is different, recalculate the filter coefficients
        if (!CoreMath::compareFloatsEqual(newSampleRate, _sampleRate)) {
            _sampleRate = newSampleRate;
            setLowCutoff(_lowCutoffHz);
            setHighCutoff(_highCutoffHz);
        }
    }

    template <typename T>
    void MONSTRBand<T>::setLowCutoff(double val) {
        // if this is the lowest band, then do not cut the low frequencies
        if (!_isLower && !_isUpper) {
            _lowCutoffHz = Parameters::CROSSOVERLOWER.BoundsCheck(val);
            _lowCut1.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
            _lowCut2.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
        } else if (_isUpper) {
            _lowCutoffHz = Parameters::CROSSOVERUPPER.BoundsCheck(val);
            _lowCut1.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
            _lowCut2.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
        }
    }

    template <typename T>
    void MONSTRBand<T>::setHighCutoff(double val) {
        // if this is the highest band, then do not cut the high frequencies
        if (!_isLower && !_isUpper) {
            _highCutoffHz = Parameters::CROSSOVERUPPER.BoundsCheck(val);
            _highCut1.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
            _highCut2.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
        } else if (_isLower) {
            _highCutoffHz = Parameters::CROSSOVERLOWER.BoundsCheck(val);
            _highCut1.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
            _highCut2.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
        }
    }

    template <typename T>
    void MONSTRBand<T>::process2in2out(T* leftSample, T* rightSample, size_t numSamples) {

        // Apply the filtering before processing
        _filterSamples(leftSample, rightSample, static_cast<int>(numSamples));

        if (_isActive) {
            // Do the actual stereo widening or narrowing
            // Based on: http://musicdsp.org/showArchiveComment.php?ArchiveID=256
            double coef_S {_width * 0.5};

            for (size_t iii {0}; iii < numSamples; iii++) {

                double mid {(leftSample[iii] + rightSample[iii]) * 0.5};
                double side {(rightSample[iii] - leftSample[iii]) * coef_S};

                leftSample[iii] = mid - side;
                rightSample[iii] = mid + side;
            }
        }
    }

    template <typename T>
    void MONSTRBand<T>::_filterSamples(T* inLeftSamples, T* inRightSamples, int numSamples) {
        T* channelsArray[2];

        channelsArray[0] = inLeftSamples;
        channelsArray[1] = inRightSamples;

        if (_isLower) {
            _highCut1.process(numSamples, channelsArray);
            _highCut2.process(numSamples, channelsArray);
        } else if (_isUpper) {
            _lowCut1.process(numSamples, channelsArray);
            _lowCut2.process(numSamples, channelsArray);
        } else {
            _lowCut1.process(numSamples, channelsArray);
            _lowCut2.process(numSamples, channelsArray);
            _highCut1.process(numSamples, channelsArray);
            _highCut2.process(numSamples, channelsArray);
        }
    }
}
