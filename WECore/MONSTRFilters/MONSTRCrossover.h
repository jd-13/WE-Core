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
#include <array>
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
         *                               must be the same size
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

        class BandWrapper {
        public:
            BandWrapper() : band(BandType::LOWER) {
                leftBuffer = new SampleType[INTERNAL_BUFFER_SIZE];
                rightBuffer = new SampleType[INTERNAL_BUFFER_SIZE];
            }

            MONSTRBand<SampleType> band;
            SampleType* leftBuffer;
            SampleType* rightBuffer;
        };

        std::array<BandWrapper, 3> _bands;
    };

    template <typename SampleType>
    MONSTRCrossover<SampleType>::MONSTRCrossover() {

        _bands[1].band.setBandType(BandType::MIDDLE, 100, 100);
        _bands[2].band.setBandType(BandType::UPPER, 100, 100);

        setCrossoverFrequency(0, 100);
        setCrossoverFrequency(1, 5000);
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::setIsActive(size_t index, bool isActive) {
        if (index < _bands.size()) {
            _bands[index].band.setIsActive(isActive);
        }
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::setCrossoverFrequency(size_t index, double val) {

        if (index < _bands.size() - 1) {

            // Set the crossover frequency
            val = Parameters::CROSSOVER_FREQUENCY.BoundsCheck(val);
            _bands[index].band.setHighCutoff(val);
            _bands[index + 1].band.setLowCutoff(val);

            // Make sure the crossover frequencies are still in the correct order
            for (size_t otherCrossoverIndex {0}; otherCrossoverIndex < _bands.size() - 1; otherCrossoverIndex++) {

                const double otherCrossoverFrequency {getCrossoverFrequency(otherCrossoverIndex)};

                const bool needsCrossoverUpdate {
                    // We've moved the crossover frequency of index below another one that should be
                    // below it - move the other one to the new value
                    (val < otherCrossoverFrequency && otherCrossoverIndex < index) ||

                    // We've moved the crossover frequency of index above another one that should be
                    // above it - move the other one to the new value
                    (otherCrossoverFrequency > val && index > otherCrossoverIndex)
                };

                if (needsCrossoverUpdate) {
                    // We've moved the crossover frequency of index below another one that should be
                    // below it - move the other one to the new value
                    setCrossoverFrequency(otherCrossoverIndex, val);

                }
            }
        }
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::setEffectsProcessor(size_t index, std::shared_ptr<EffectsProcessor<SampleType>> processor) {

        if (index < _bands.size()) {
            _bands[index].band.setEffectsProcessor(processor);
        }
    }


    template <typename SampleType>
    void MONSTRCrossover<SampleType>::setSampleRate(double newSampleRate) {
        for (BandWrapper band : _bands) {
            band.band.setSampleRate(newSampleRate);
        }
    }

    template <typename SampleType>
    bool MONSTRCrossover<SampleType>::getIsActive(size_t index) const {
        bool retVal {false};

        if (index < _bands.size()) {
            retVal = _bands[index].band.getIsActive();
        }

        return retVal;
    }

    template <typename SampleType>
    double MONSTRCrossover<SampleType>::getCrossoverFrequency(size_t index) const {
        double retVal {0};

        if (index < _bands.size()) {
            retVal = _bands[index].band.getHighCutoff();
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
            const size_t numSamplesToCopy {
                std::min(numSamplesRemaining, static_cast<size_t>(INTERNAL_BUFFER_SIZE))
            };

            SampleType* const leftBufferInputStart {&leftSample[bufferNumber * INTERNAL_BUFFER_SIZE]};
            SampleType* const rightBufferInputStart {&rightSample[bufferNumber * INTERNAL_BUFFER_SIZE]};

            // Populate the band specific buffers, and do the processing
            for (size_t bandIndex {0}; bandIndex < _bands.size(); bandIndex++) {
                std::copy(leftBufferInputStart,
                          &leftBufferInputStart[numSamplesToCopy],
                          _bands[bandIndex].leftBuffer);

                std::copy(rightBufferInputStart,
                          &rightBufferInputStart[numSamplesToCopy],
                          _bands[bandIndex].rightBuffer);

                _bands[bandIndex].band.process2in2out(_bands[bandIndex].leftBuffer,
                                                      _bands[bandIndex].rightBuffer,
                                                      numSamplesToCopy);
            }

            // Combine the output from each band, and write to output
            for (size_t sampleIndex {0}; sampleIndex < numSamplesToCopy; sampleIndex++) {

                // Reset the sample to zero first
                leftBufferInputStart[sampleIndex] = 0;
                rightBufferInputStart[sampleIndex] = 0;

                for (size_t bandIndex {0}; bandIndex < _bands.size(); bandIndex++) {
                    leftBufferInputStart[sampleIndex] += _bands[bandIndex].leftBuffer[sampleIndex];
                    rightBufferInputStart[sampleIndex] += _bands[bandIndex].rightBuffer[sampleIndex];
                }
            }
        }
    }

    template <typename SampleType>
    void MONSTRCrossover<SampleType>::reset() {
        for (BandWrapper& band : _bands) {
            band.band.reset();
        }
    }
}
