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
    template <typename T>
    class MONSTRCrossover {
        static_assert(std::is_floating_point<T>::value,
                      "Must be provided with a floating point template type");

    public:
        MONSTRBand<T>  band1,
                       band2,
                       band3;

        /**
         * Makes each band aware of its position, and therefore which of their internal filters
         * they each need to activate.
         */
        MONSTRCrossover() : band1(BandType::LOWER),
                            band2(BandType::MIDDLE),
                            band3(BandType::UPPER) {
            setCrossoverLower(100);
            setCrossoverUpper(5000);
        }

        virtual ~MONSTRCrossover() {}

        /**
         * Applies the filtering to a stereo buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param[out]   leftSample      Pointer to the first sample of the left channel's buffer
         * @param[out]   rightSample     Pointer to the first sample of the right channel's buffer
         * @param[in]    numSamples      Number of samples in the buffer. The left and right buffers
         *                               must be the same size.
         */
        inline void Process2in2out(T* leftSample, T* rightSample, size_t numSamples);

        /**
         * Sets the crossover frequency of the lower (band1) and middle (band2) bands.
         *
         * @param   val   The frequency in Hz to set the lower crossover point to.
         *
         * @see     CROSSOVERLOWER for valid values
         */
        inline void setCrossoverLower(double val);

        /**
         * Sets the crossover frequency of the middle (band2) and upper (band3) bands.
         *
         * @param   val   The frequency in Hz to set the upper crossover point to.
         *
         * @see     CROSSOVERUPPER for valid values
         */
        inline void setCrossoverUpper(double val);

        /**
         * Gets the crossover frequency of the lower (band1) and middle (band2) bands.
         *
         * @return  val The frequency in Hz of the lower crossover point.
         *
         * @see     CROSSOVERLOWER for valid values
         */
        double getCrossoverLower() { return band1.getHighCutoff(); }

        /**
         * Gets the crossover frequency of the middle (band2) and upper (band3) bands.
         *
         * @return  val The frequency in Hz of the upper crossover point.
         *
         * @see     CROSSOVERUPPER for valid values
         */
        double getCrossoverUpper() { return band2.getHighCutoff(); }

        /**
         * Configures the filters for the correct sample rate. Ensure this is
         * called before attempting to process audio.
         *
         * @param   newSampleRate  The sample rate the filter should be configured for
         */
        inline void setSampleRate(double newSampleRate);

        /**
         * Resets all filters.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        inline void reset();

    private:
        static constexpr unsigned int INTERNAL_BUFFER_SIZE = 512;

        T _band1LeftBuffer[INTERNAL_BUFFER_SIZE];
        T _band1RightBuffer[INTERNAL_BUFFER_SIZE];
        T _band2LeftBuffer[INTERNAL_BUFFER_SIZE];
        T _band2RightBuffer[INTERNAL_BUFFER_SIZE];
        T _band3LeftBuffer[INTERNAL_BUFFER_SIZE];
        T _band3RightBuffer[INTERNAL_BUFFER_SIZE];
    };

    template <typename T>
    void MONSTRCrossover<T>::Process2in2out(T* leftSample,  T* rightSample, size_t numSamples) {

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

            T* const leftBufferInputStart {&leftSample[bufferNumber * INTERNAL_BUFFER_SIZE]};
            T* const rightBufferInputStart {&rightSample[bufferNumber * INTERNAL_BUFFER_SIZE]};

            std::copy(leftBufferInputStart,  &leftBufferInputStart[numSamplesToCopy],  _band1LeftBuffer);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _band1RightBuffer);
            std::copy(leftBufferInputStart,  &leftBufferInputStart[numSamplesToCopy],  _band2LeftBuffer);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _band2RightBuffer);
            std::copy(leftBufferInputStart,  &leftBufferInputStart[numSamplesToCopy],  _band3LeftBuffer);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _band3RightBuffer);

            // let each band do its processing
            band1.process2in2out(_band1LeftBuffer, _band1RightBuffer, numSamplesToCopy);
            band2.process2in2out(_band2LeftBuffer, _band2RightBuffer, numSamplesToCopy);
            band3.process2in2out(_band3LeftBuffer, _band3RightBuffer, numSamplesToCopy);

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

    template <typename T>
    void MONSTRCrossover<T>::setCrossoverLower(double val) {
        val = Parameters::CROSSOVER_FREQUENCY.BoundsCheck(val);
        band1.setHighCutoff(val);
        band2.setLowCutoff(val);

        // Move the high crossover up if necessary as they shouldn't swap places
        if (val > getCrossoverUpper()) {
            setCrossoverUpper(val);
        }
    }

    template <typename T>
    void MONSTRCrossover<T>::setCrossoverUpper(double val) {
        val = Parameters::CROSSOVER_FREQUENCY.BoundsCheck(val);
        band2.setHighCutoff(val);
        band3.setLowCutoff(val);

        // Move the low crossover down if necessary as they shouldn't swap places
        if (val < getCrossoverLower()) {
            setCrossoverLower(val);
        }
    }

    template <typename T>
    void MONSTRCrossover<T>::setSampleRate(double newSampleRate) {
        band1.setSampleRate(newSampleRate);
        band2.setSampleRate(newSampleRate);
        band3.setSampleRate(newSampleRate);
    }

    template <typename T>
    void MONSTRCrossover<T>::reset() {
        band1.reset();
        band2.reset();
        band3.reset();
    }
}
