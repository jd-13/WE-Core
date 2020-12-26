/*
 *  File:       MONSTRCrossover.h
 *
 *  Version:    1.0.0
 *
 *  Created:    08/11/2016
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

#pragma once

#include <algorithm>
#include "MONSTRBand.h"

namespace WECore::MONSTR {
    /**
     * Class which provides three band stereo width control.
     *
     * This class contains three MONSTRBand objects, and may later be expanded
     * to support a variable number of bands.
     *
     * To use this class, use the setter and getter methods to manipulate crossover
     * parameters which are shared by several bands (crossover frequencies), and call
     * the setter and getter methods of each band to manipulate parameters which are
     * particular to each band. Call the reset, setSampleRate and process methods as
     * required.
     *
     * @see MONSTRBand
     *
     * The below example shows how to prepare a MONSTRCrossover object to process audio:
     * @code
     * MONSTRCrossover crossover;
     * crossover.setSampleRate(44100);
     * @endcode
     *
     * Then when you have a buffer ready:
     * @code
     * crossover.Process2in2out(leftSample, rightSample, numSamples);
     * @endcode
     */
    template <typename SampleType>
    class MONSTRCrossover {
        static_assert(std::is_floating_point<SampleType>::value,
                      "Must be provided with a floating point template type");

    public:

        MONSTRCrossover();
        virtual ~MONSTRCrossover() = default;

        /** @name Setter Methods */
        /** @{ */

        /**
         * Activates or bypasses the processing of the given band.
         *
         * @param   index       The band to set
         * @param   isActive    Set to true if the band's processing should be active
         */
        inline void setIsActive(size_t index, bool isActive);

        /**
         * Sets the crossover frequency of the band at the provided index.
         *
         * @param   index The crossover to set
         * @param   val   The frequency in Hz to set the crossover point to
         *
         * @see     CROSSOVER_FREQUENCY for valid values
         */
        inline void setCrossoverFrequency(size_t index, double val);

        /**
         * Sets the effects processor which will be used by the given band.
         *
         * @param   index       The band to set
         * @param   processor   EffectsProcessor to use
         */
        inline void setEffectsProcessor(
            size_t index, std::shared_ptr<EffectsProcessor<SampleType>> processor);

        /**
         * Configures the filters for the correct sample rate. Ensure this is
         * called before attempting to process audio.
         *
         * @param   newSampleRate  The sample rate the filter should be configured for
         */
        inline void setSampleRate(double newSampleRate);

        /** @} */

        /** @name Getter Methods */
        /** @{ */

        /**
         * Gets whether the given band is applying processing.
         *
         * @param   index The band to get
         *
         * @return  True if the band is applying processing, false if bypassed
         */
        inline bool getIsActive(size_t index) const;

        /**
         * Gets the crossover frequency of the band at the provided index.
         *
         * @param   index The crossover to get.
         *
         * @return  The frequency in Hz of the lower crossover point
         *
         * @see     CROSSOVER_FREQUENCY for valid values
         */
        inline double getCrossoverFrequency(size_t index) const;

        /** @} */

        /**
         * Applies the filtering to a stereo buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param[out]   leftSample      Pointer to the first sample of the left channel's buffer
         * @param[out]   rightSample     Pointer to the first sample of the right channel's buffer
         * @param[in]    numSamples      Number of samples in the buffer. The left and right buffers
         *                               must be the same size.
         */
        inline void Process2in2out(SampleType* leftSample,
                                   SampleType* rightSample,
                                   size_t numSamples);

        /**
         * Resets all filters.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        inline void reset();

    private:
        static constexpr unsigned int INTERNAL_BUFFER_SIZE = 512;

        MONSTRBand<SampleType>  _band1,
                                _band2,
                                _band3;

        SampleType _band1LeftBuffer[INTERNAL_BUFFER_SIZE];
        SampleType _band1RightBuffer[INTERNAL_BUFFER_SIZE];
        SampleType _band2LeftBuffer[INTERNAL_BUFFER_SIZE];
        SampleType _band2RightBuffer[INTERNAL_BUFFER_SIZE];
        SampleType _band3LeftBuffer[INTERNAL_BUFFER_SIZE];
        SampleType _band3RightBuffer[INTERNAL_BUFFER_SIZE];
    };

    template <typename SampleType>
    MONSTRCrossover<SampleType>::MONSTRCrossover() : _band1(BandType::LOWER),
                                                     _band2(BandType::MIDDLE),
                                                     _band3(BandType::UPPER) {
        setCrossoverFrequency(0, 100);
        setCrossoverFrequency(1, 5000);
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::setIsActive(size_t index, bool isActive) {
        if (index == 0) {
            _band1.setIsActive(isActive);
        } else if (index == 1) {
            _band2.setIsActive(isActive);
        } else if (index == 2) {
            _band3.setIsActive(isActive);
        }
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::setCrossoverFrequency(size_t index, double val) {
        val = Parameters::CROSSOVER_FREQUENCY.BoundsCheck(val);

        if (index == 0) {
            _band1.setHighCutoff(val);
            _band2.setLowCutoff(val);
        } else if (index == 1) {
            _band2.setHighCutoff(val);
            _band3.setLowCutoff(val);
        }

        // Move the other crossovers if needed as they should never swap places
        if (getCrossoverFrequency(0) > getCrossoverFrequency(1)) {
            if (index == 0) {
                setCrossoverFrequency(1, val);
            } else if (index == 1) {
                setCrossoverFrequency(0, val);
            }
        }
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::setEffectsProcessor(size_t index, std::shared_ptr<EffectsProcessor<SampleType>> processor) {
        if (index == 0) {
            _band1.setEffectsProcessor(processor);
        } else if (index == 1) {
            _band2.setEffectsProcessor(processor);
        } else if (index == 2) {
            _band3.setEffectsProcessor(processor);
        }
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::setSampleRate(double newSampleRate) {
        _band1.setSampleRate(newSampleRate);
        _band2.setSampleRate(newSampleRate);
        _band3.setSampleRate(newSampleRate);
    }

    template <typename SampleType>
    bool MONSTRCrossover<SampleType>::getIsActive(size_t index) const {
        bool retVal {false};

        if (index == 0) {
            retVal = _band1.getIsActive();
        } else if (index == 1) {
            retVal = _band2.getIsActive();
        } else if (index == 2) {
            retVal = _band3.getIsActive();
        }

        return retVal;
    }

    template <typename SampleType>
    double MONSTRCrossover<SampleType>::getCrossoverFrequency(size_t index) const {
        double retVal {0};

        if (index == 0) {
            retVal = _band1.getHighCutoff();
        } else if (index == 1) {
            retVal = _band2.getHighCutoff();
        }

        return retVal;
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::Process2in2out(SampleType* leftSample,
                                                     SampleType* rightSample,
                                                     size_t numSamples) {

        // If the buffer we've been passed is bigger than our static internal buffer, then we need
        // to break it into chunks
        const size_t numBuffersRequired {static_cast<size_t>(
            std::ceil(static_cast<double>(numSamples) / INTERNAL_BUFFER_SIZE)
            )};

        for (size_t bufferNumber {0}; bufferNumber < numBuffersRequired; bufferNumber++) {

            // Calculate how many samples need to be processed in this chunk
            const size_t numSamplesRemaining {numSamples - (bufferNumber * INTERNAL_BUFFER_SIZE)};
            const size_t numSamplesToCopy {std::min(numSamplesRemaining,
                                        static_cast<size_t>(INTERNAL_BUFFER_SIZE))};

            SampleType* const leftBufferInputStart {&leftSample[bufferNumber * INTERNAL_BUFFER_SIZE]};
            SampleType* const rightBufferInputStart {&rightSample[bufferNumber * INTERNAL_BUFFER_SIZE]};

            std::copy(leftBufferInputStart,  &leftBufferInputStart[numSamplesToCopy],  _band1LeftBuffer);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _band1RightBuffer);
            std::copy(leftBufferInputStart,  &leftBufferInputStart[numSamplesToCopy],  _band2LeftBuffer);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _band2RightBuffer);
            std::copy(leftBufferInputStart,  &leftBufferInputStart[numSamplesToCopy],  _band3LeftBuffer);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _band3RightBuffer);

            // let each band do its processing
            _band1.process2in2out(_band1LeftBuffer, _band1RightBuffer, numSamplesToCopy);
            _band2.process2in2out(_band2LeftBuffer, _band2RightBuffer, numSamplesToCopy);
            _band3.process2in2out(_band3LeftBuffer, _band3RightBuffer, numSamplesToCopy);

            // combine the output from each band, and write to output
            for (size_t iii {0}; iii < numSamplesToCopy; iii++) {
                leftBufferInputStart[iii] = _band1LeftBuffer[iii]
                                            + _band2LeftBuffer[iii]
                                            + _band3LeftBuffer[iii];

                rightBufferInputStart[iii] = _band1RightBuffer[iii]
                                            + _band2RightBuffer[iii]
                                            + _band3RightBuffer[iii];
            }
        }
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::reset() {
        _band1.reset();
        _band2.reset();
        _band3.reset();
    }
}
