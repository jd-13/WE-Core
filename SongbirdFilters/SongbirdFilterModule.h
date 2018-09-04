/*
 *  File:       SongbirdFilterModule.h
 *
 *  Version:    1.0.0
 *
 *  Created:    12/06/2016
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

#include "SongbirdFormantFilter.h"
#include <map>
#include "SongbirdFiltersParameters.h"
#include <array>

namespace {
    /**
     * The number of formants (bandpass filters) which are used in a single vowel.
     */
    static const int NUM_FORMANTS_PER_VOWEL {5};

    /**
     * The number of vowels supported.
     */
    static const int NUM_VOWELS {5};
}

namespace WECore::Songbird {
    /**
     * A type to make refering to a group of formants easier.
     */
    typedef std::array<Formant, NUM_VOWELS> Vowel;

    /**
     * Enums used to make identifing left or right channel filters easier
     */
    enum class Channels {
        LEFT,
        RIGHT
    };

    /**
     * A filter module providing five different vowel sounds, any two of which can be selected
     * simulaneously and blended between.
     *
     * To use this class, simply call reset, and the process methods as necessary, using the provided
     * getter and setter methods to manipulate parameters.
     *
     * You must call setSampleRate before beginning any processing as the default sample rate might not
     * be the one you want.
     *
     * Internally relies on the parameters provided in SongbirdFiltersParameters.h
     *
     * @see SongbirdFormantFilter   - SongbirdFilterModule is composed of two pairs of
     *                                SongbirdFormantFilters (pairs to allow stereo processing), each
     *                                pair is assigned one of the five supported vowels at any time
     *
     * A SongbirdFilterModule can be created and used to process a buffer as follows:
     * @code
     * SongbirdFilterModule filter;
     * filter.setSampleRate(44100);
     * filter.setVowel1(VOWEL.VOWEL_O);
     * filter.setVowel2(VOWEL.VOWEL_I);
     * ...
     * set any other parameters you need
     * ...
     *
     * filter.Process2in2out(leftSamples, rightSamples, numSamples);
     * @endcode
     */
    class SongbirdFilterModule {
    public:
        /**
         * Does some basic setup and defaulting of parameters, though do not rely on this
         * to sensibly default all parameters.
         */
        SongbirdFilterModule() : _vowel1(Parameters::VOWEL.VOWEL_A),
                                 _vowel2(Parameters::VOWEL.VOWEL_E),
                                 _filterPosition(Parameters::FILTER_POSITION.defaultValue),
                                 _sampleRate(44100),
                                 _mix(Parameters::MIX.defaultValue),
                                 _modulationSrc(Parameters::MODULATION.defaultValue),
                                 _modMode(Parameters::MODMODE_DEFAULT) {

            // initialise the filters to some default values
            setVowel1(_vowel1);
            setVowel2(_vowel2);
        }

        virtual ~SongbirdFilterModule() {}

        /** @name Setter Methods */
        /** @{ */

        /**
         * Sets the vowel sound that should be created by filter 1 using one of
         * the built in Vowel objects stored in this class.
         *
         * @param[in]   val Value that should be used for Vowel 1
         *
         * @see         VowelParameter for valid values
         */
        inline void setVowel1(int val);

        /**
         * Sets the vowel sound that should be created by filter 2.
         *
         * @param[in]   val Value that should be used for Vowel 2
         *
         * @see         VowelParameter for valid values
         */
        inline void setVowel2(int val);

        /**
         * Sets the position between the two filters that have been selected.
         *
         * @param[in]   val Filter position to use.
         *
         * @see         FILTER_POSITION for valid values
         * @see         modMode which the effect of this parameter is dependent on

        */
        void setFilterPosition(double val) { _filterPosition = Parameters::FILTER_POSITION.BoundsCheck(val); }

        /**
         * Set the sample rate that the filters expect of the audio which will be processed.
         *
         * @param[in]   val The sample rate to set the filters to
         */
        inline void setSampleRate(double val);

        /**
         * Sets the dry/wet mix level.
         * Lowest value = completely dry, unprocessed signal, no filtering applied.
         * Highest value = completely wet signal, no unprocessed audio survives.
         *
         * @param[in]   val Mix value that should be used
         *
         * @see         MIX for valid values
         */
        void setMix(double val) { _mix = Parameters::MIX.BoundsCheck(val); }

        /**
         * Sets the modulation position. Usually driven by an LFO or envelope, use values between -1 and
         * 1.
         *
         * @param[in]   val The output of the modulation source (LFO, envelope, etc)
         */
        void setModulation(float val) { _modulationSrc = Parameters::MODULATION.BoundsCheck(val); }

        /**
         * Sets the modulation mode to apply to the filters.
         * 
         * @param[in]   val Chooses the modulation mode
         */
        void setModMode(bool val) { _modMode = val; }
        
        /** @} */

        /**
         * Resets all filters.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        inline void reset();

        /** @name Getter Methods */
        /** @{ */

        /**
         * @see setVowel1
         */
        int getVowel1() { return _vowel1; }

        /**
         * @see setVowel2
         */
        int getVowel2() { return _vowel2; }

        /**
         * Return a vowel object describing one of the built in vowels.
         *
         * @see     VowelParameter for valid values
         */
        inline Vowel getVowelDescription(int val);

        /**
         * @see setFilterPosition
         */
        double getFilterPosition() { return _filterPosition; }

        /**
         * @see setMix
         */
        double getMix() { return _mix; }

        /**
         * @see modMode
         */
        bool getModMode() { return _modMode; }
        
        /** @} */

        /**
         * Applies the filtering to a stereo buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param   leftSamples    Pointer to the first sample of the left channel's buffer
         * @param   rightSamples   Pointer to the first sample of the right channel's buffer
         * @param   numSamples     Number of samples in the buffer. The left and right buffers
         *                         must be the same size.
         */
        inline void Process2in2out(double* leftSamples,
                                   double* rightSamples,
                                   size_t numSamples);

        SongbirdFilterModule operator=(SongbirdFilterModule& other) = delete;
        SongbirdFilterModule(SongbirdFilterModule& other) = delete;

    private:
        int _vowel1,
            _vowel2;

        double  _filterPosition,
                _sampleRate,
                _mix,
                _modulationSrc;

        bool _modMode;

        std::map<Channels, SongbirdFormantFilter> _filters1;
        std::map<Channels, SongbirdFormantFilter> _filters2;

        static constexpr unsigned int INTERNAL_BUFFER_SIZE = 512;

        double _leftOutputBuffer1[INTERNAL_BUFFER_SIZE];
        double _rightOutputBuffer1[INTERNAL_BUFFER_SIZE];
        double _leftOutputBuffer2[INTERNAL_BUFFER_SIZE];
        double _rightOutputBuffer2[INTERNAL_BUFFER_SIZE];

        /**
         * Sets the vowel sound that should be created by filter 1 using a Vowel object provided by the
         * caller rather than one of the built in Vowel objects stored in this class.
         *
         * @param   val Value that should be used for Vowel 1
         */
        inline void _setVowel1(Vowel val);

        /**
         * Uses the filterPosition parameter and the modulation source to calculate the vowel that
         * should be used when in MODMODE_FREQ, as this vowel will sit somewhere between the two vowels
         * that have been selected by the user.
         */
        inline Vowel _calcVowelForFreqMode();

        /**
         * An array which defines all the formants that will be needed.
         */
        // (TODO: could be made static again)
        const Formant _allFormants[NUM_VOWELS][NUM_FORMANTS_PER_VOWEL] {
            {Formant(800, 0), Formant(1150, -4), Formant(2800, -20), Formant(3500, -36), Formant(4950, -60)},
            {Formant(400, 0), Formant(1600, -24), Formant(2700, -30), Formant(3300, -35), Formant(4950, -60)},
            {Formant(350, 0), Formant(1700, -20), Formant(2700, -30), Formant(3700, -36), Formant(4950, -60)},
            {Formant(450, 0), Formant(800, -9), Formant(2830, -16), Formant(3500, -28), Formant(4950, -55)},
            {Formant(325, 0), Formant(700, -12), Formant(2530, -30), Formant(3500, -40), Formant(4950, -64)},
        };
    };

    void SongbirdFilterModule::setVowel1(int val) {
        // perform a bounds check, then apply the appropriate formants
        _vowel1 = Parameters::VOWEL.BoundsCheck(val);

        const std::vector<Formant> tempFormants(&_allFormants[_vowel1 - 1][0],
                                                &_allFormants[_vowel1 - 1][NUM_FORMANTS_PER_VOWEL]);

        _filters1[Channels::LEFT].setFormants(tempFormants);
        _filters1[Channels::RIGHT].setFormants(tempFormants);
    }

    void SongbirdFilterModule::setVowel2(int val) {
        // perform a bounds check, then apply the appropriate formants
        _vowel2 = Parameters::VOWEL.BoundsCheck(val);

        const std::vector<Formant> tempFormants(&_allFormants[_vowel2 - 1][0],
                                                &_allFormants[_vowel2 - 1][NUM_FORMANTS_PER_VOWEL]);
        _filters2[Channels::LEFT].setFormants(tempFormants);
        _filters2[Channels::RIGHT].setFormants(tempFormants);
    }

    void SongbirdFilterModule::setSampleRate(double val) {
        _sampleRate = val;

        _filters1[Channels::LEFT].setSampleRate(val);
        _filters1[Channels::RIGHT].setSampleRate(val);
        _filters2[Channels::LEFT].setSampleRate(val);
        _filters2[Channels::RIGHT].setSampleRate(val);
    }

    void SongbirdFilterModule::reset() {
        _filters1[Channels::LEFT].reset();
        _filters1[Channels::RIGHT].reset();
        _filters2[Channels::LEFT].reset();
        _filters2[Channels::RIGHT].reset();
    }

    Vowel SongbirdFilterModule::getVowelDescription(int val) {
        Vowel tempVowel;

        std::copy(&_allFormants[val - 1][0],
                  &_allFormants[val - 1][NUM_FORMANTS_PER_VOWEL],
                  std::begin(tempVowel));

        return tempVowel;
    }

    void SongbirdFilterModule::Process2in2out(double* leftSamples,
                                              double* rightSamples,
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

            // Get a pointer to the start of this chunk
            double* const leftBufferInputStart {&leftSamples[bufferNumber * INTERNAL_BUFFER_SIZE]};
            double* const rightBufferInputStart {&rightSamples[bufferNumber * INTERNAL_BUFFER_SIZE]};

            // Copy the samples we need to process in this chunk into the internal buffers
            std::copy(leftBufferInputStart, &leftBufferInputStart[numSamplesToCopy], _leftOutputBuffer1);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _rightOutputBuffer1);

            std::copy(leftBufferInputStart, &leftBufferInputStart[numSamplesToCopy], _leftOutputBuffer2);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _rightOutputBuffer2);

            // Figure out the modulation here. We have two ways to modulation between
            // two formant filters.
            // For MODMODE_BLEND we modulation the filter position to blend between the two filters.
            // For MODMOD_FREQ we set the filter position to 0 so that we're only using filter 1,
            // and then modulate the freqency of filter 1 between the two vowels
            double blendFilterPosition;
            if (_modMode == Parameters::MODMODE_BLEND) {
                blendFilterPosition = _filterPosition + _modulationSrc;
            } else {
                blendFilterPosition = 0;
                _setVowel1(_calcVowelForFreqMode());
            }

            // Do the processing for each filter
            _filters1[Channels::LEFT].process(_leftOutputBuffer1, numSamplesToCopy);
            _filters1[Channels::RIGHT].process(_rightOutputBuffer1, numSamplesToCopy);

            _filters2[Channels::LEFT].process(_leftOutputBuffer2, numSamplesToCopy);
            _filters2[Channels::RIGHT].process(_rightOutputBuffer2, numSamplesToCopy);

            // Write to output, applying filter position and mix level
            // always use modFilterPosition as this will take into account any modulation
            for (size_t iii {0}; iii < numSamplesToCopy; iii++) {
                leftBufferInputStart[iii] = leftBufferInputStart[iii] * (1 - _mix)
                                            + _leftOutputBuffer1[iii] * (1 - blendFilterPosition) * _mix
                                            + _leftOutputBuffer2[iii] * blendFilterPosition * _mix;

                rightBufferInputStart[iii] = rightBufferInputStart[iii] * (1 - _mix)
                                            + _rightOutputBuffer1[iii] * (1 - blendFilterPosition) * _mix
                                            + _rightOutputBuffer2[iii] * blendFilterPosition * _mix;
            }
        }
    }

    void SongbirdFilterModule::_setVowel1(Vowel val) {
        const std::vector<Formant> tempFormants(val.begin(), val.end());

        _filters1[Channels::LEFT].setFormants(tempFormants);
        _filters1[Channels::RIGHT].setFormants(tempFormants);
    }

    Vowel SongbirdFilterModule::_calcVowelForFreqMode() {
        // get the first and second vowels
        Vowel tempVowel1(getVowelDescription(getVowel1()));
        Vowel tempVowel2(getVowelDescription(getVowel2()));


        Vowel retVal(tempVowel1);

        for (size_t iii {0}; iii < NUM_FORMANTS_PER_VOWEL; iii++) {
            // TODO: There's definitely some optimisation to be done below

            // Calculate frequency modulation
            double freqDelta {std::fabs(tempVowel1[iii].frequency - tempVowel2[iii].frequency)};
            // Invert the delta depending on which value is largest
            freqDelta *= (tempVowel1[iii].frequency > tempVowel2[iii].frequency) ? -1.0 : 1.0;

            retVal[iii].frequency = tempVowel1[iii].frequency + freqDelta / 2;
            retVal[iii].frequency += (freqDelta / 2) * _modulationSrc
                                    + (freqDelta / 2) * ((_filterPosition - 0.5) * 2);

            // Calculate gain modulation
            double gainDelta {std::fabs(tempVowel1[iii].gaindB - tempVowel2[iii].gaindB)};
            // Invert the delta depending on which value is largest
            gainDelta *= (tempVowel1[iii].gaindB > tempVowel2[iii].gaindB) ? -1.0 : 1.0;

            retVal[iii].gaindB = tempVowel1[iii].gaindB + gainDelta / 2;
            retVal[iii].gaindB += (gainDelta / 2) * _modulationSrc
                                + (gainDelta / 2) * ((_filterPosition - 0.5) * 2);
        }

        return retVal;
    }
}
