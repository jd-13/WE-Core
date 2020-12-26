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

#include <memory>
#include "MONSTRParameters.h"
#include "General/CoreMath.h"
#include "WEFilters/EffectsProcessor.h"

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
     * Possible types of MONSTRBand needed for a crossover.
     */
    enum class BandType {
        LOWER,
        MIDDLE,
        UPPER
    };

    /**
     * Provides processing for single frequency range in a crossover.
     *
     * It is recommended to use this class through the MONSTRCrossover class.
     *
     * This class contains a highpass and a lowpass filter in series (this is a roundabout way of
     * creating a bandpass but this appeared to perform better), the passband can then be processed
     * by a provided EffectsProcessor
     *
     * Internally relies on the parameters provided in MONSTRParameters.h
     *
     * @see MONSTRCrossover
     */
    template <typename SampleType>
    class MONSTRBand {
        static_assert(std::is_floating_point<SampleType>::value,
                "Must be provided with a floating point template type");

    public:

        /**
         * Performs setup of the filters using default settings. The arguments are
         * used to tell the MONSTRBand object which part of the frequency spectrum
         * it will be covering as crossover filter.
         *
         * NOTE: The parameters cannot both be true. This will cause undefined behaviour.
         *
         * @param[in]   bandType    Tells the band whether it is at either end of the crossover or
         *                          in the middle.
         *
         */
        MONSTRBand(BandType bandType) : _bandType(bandType),
                                        _isActive(Parameters::BANDSWITCH_DEFAULT),
                                        _lowCutoffHz(100),
                                        _highCutoffHz(5000),
                                        _sampleRate(44100),
                                        _processor(nullptr) {
            _lowCut1.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
            _lowCut2.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
            _highCut1.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
            _highCut2.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
        }

        virtual ~MONSTRBand() = default;

        /** @name Setter Methods */
        /** @{ */

        /**
         * Sets whether this band will route audio through the EffectsProcessor.
         *
         * If the band is deactivated filtering will still be performed. This is so that a
         * crossover containing multiple MONSTRBands will always be able easily to sum the
         * output of the bands to unity gain, whether some of the MONSTRBands are bypassed
         * or not.
         *
         * @param   val If true, stereo width processing will be applied
         */
        void setIsActive(bool val) { _isActive = val; }

        inline void setLowCutoff(double val);

        inline void setHighCutoff(double val);

        inline void setBandType(BandType bandType, double lowCutoff, double highCutoff);

        inline void setSampleRate(double newSampleRate);

        /**
         * Specifies the processor that this band should pass audio through.
         */
        void setEffectsProcessor(std::shared_ptr<EffectsProcessor<SampleType>> processor) { _processor = processor; }

        /** @} */

        /** @name Getter Methods */
        /** @{ */

        /**
         * @see setIsActive
         */
        bool getIsActive() const { return _isActive; }

        double getLowCutoff() const { return _lowCutoffHz; }

        double getHighCutoff() const { return _highCutoffHz; }

        /** @} */

        /**
         * Performs the effect processing on leftSample and rightSample. Use for
         * stereo in->stereo out signals.
         *
         * @param[out]   leftSample   Pointer to the first sample of the left channel's buffer
         * @param[out]   rightSample  Pointer to the first sample of the right channel's buffer
         * @param[in]    numSamples   Number of samples in the buffer. The left and right buffers
         *                            must be the same size.
         */
        inline void process2in2out(SampleType* leftSample, SampleType* rightSample, size_t numSamples);

        /**
         * Resets filter states. Call before beginning a new buffer of samples.
         */
        inline void reset();

    private:
        BandType _bandType;

        bool _isActive;

        double   _lowCutoffHz,
                 _highCutoffHz;

        double _sampleRate;

        std::shared_ptr<EffectsProcessor<SampleType>> _processor;

        static constexpr int FILTER_ORDER {2};

        Dsp::SimpleFilter<Dsp::Butterworth::HighPass<FILTER_ORDER>, 2> _lowCut1;
        Dsp::SimpleFilter<Dsp::Butterworth::HighPass<FILTER_ORDER>, 2> _lowCut2;
        Dsp::SimpleFilter<Dsp::Butterworth::LowPass<FILTER_ORDER>, 2> _highCut1;
        Dsp::SimpleFilter<Dsp::Butterworth::LowPass<FILTER_ORDER>, 2> _highCut2;

        inline void _filterSamples(SampleType* inLeftSamples, SampleType* inRightSamples, int numSamples);
    };

    template <typename SampleType>
    void MONSTRBand<SampleType>::setLowCutoff(double val) {
        _lowCutoffHz = Parameters::CROSSOVER_FREQUENCY.BoundsCheck(val);
        _lowCut1.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);
        _lowCut2.setup(FILTER_ORDER, _sampleRate, _lowCutoffHz);

        // Move the high cutoff up if necessary as they shouldn't swap places
        if (_lowCutoffHz > _highCutoffHz) {
            setHighCutoff(_lowCutoffHz);
        }
    }

    template <typename SampleType>
    void MONSTRBand<SampleType>::setHighCutoff(double val) {
        _highCutoffHz = Parameters::CROSSOVER_FREQUENCY.BoundsCheck(val);
        _highCut1.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);
        _highCut2.setup(FILTER_ORDER, _sampleRate, _highCutoffHz);

        // Move the low cutoff down if necessary as they shouldn't swap places
        if (_lowCutoffHz > _highCutoffHz) {
            setLowCutoff(_highCutoffHz);
        }
    }

    template <typename SampleType>
    void MONSTRBand<SampleType>::setBandType(BandType bandType,
                                             double lowCutoff,
                                             double highCutoff) {
        _bandType = bandType;
        setLowCutoff(lowCutoff);
        setHighCutoff(highCutoff);
        reset();
    }

    template <typename SampleType>
    void MONSTRBand<SampleType>::setSampleRate(double newSampleRate) {
        // if the new sample rate is different, recalculate the filter coefficients
        if (!CoreMath::compareFloatsEqual(newSampleRate, _sampleRate)) {
            _sampleRate = newSampleRate;
            setLowCutoff(_lowCutoffHz);
            setHighCutoff(_highCutoffHz);
        }
    }

    template <typename SampleType>
    void MONSTRBand<SampleType>::process2in2out(SampleType* leftSample,
                                                SampleType* rightSample,
                                                size_t numSamples) {

        // Apply the filtering before processing
        _filterSamples(leftSample, rightSample, static_cast<int>(numSamples));

        if (_isActive) {
            if (_processor != nullptr) {
                _processor->process2in2out(leftSample, rightSample, numSamples);
            }
        }
    }

    template <typename SampleType>
    void MONSTRBand<SampleType>::reset() {
        _lowCut1.reset();
        _lowCut2.reset();
        _highCut1.reset();
        _highCut2.reset();
    }

    template <typename SampleType>
    void MONSTRBand<SampleType>::_filterSamples(SampleType* inLeftSamples, SampleType* inRightSamples, int numSamples) {
        SampleType* channelsArray[2];

        channelsArray[0] = inLeftSamples;
        channelsArray[1] = inRightSamples;

        if (_bandType == BandType::LOWER) {
            _highCut1.process(numSamples, channelsArray);
            _highCut2.process(numSamples, channelsArray);
        } else if (_bandType == BandType::UPPER) {
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
