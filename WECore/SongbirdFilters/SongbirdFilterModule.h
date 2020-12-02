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

#include <array>
#include <map>
#include <memory>

#include "SongbirdFormantFilter.h"
#include "SongbirdFiltersParameters.h"
#include "WEFilters/ModulationSource.h"

namespace WECore::Songbird {
    /**
     * The number of formants (bandpass filters) which are used in a single vowel.
     */
    constexpr int NUM_FORMANTS_PER_VOWEL {2};

    /**
     * The number of vowels supported.
     */
    constexpr int NUM_VOWELS {5};

    /**
     * The number of formants that are used for the "air" filters that add brightness.
     */
    constexpr int NUM_AIR_FORMANTS {2};

    /**
     * A type to make refering to a group of formants easier.
     */
    typedef std::array<Formant, NUM_FORMANTS_PER_VOWEL> Vowel;

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
     * Also provides an "air" function which is a pair of fixed frequency formant filters at higher
     * frequency that can be blended into the output.
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
    template <typename T>
    class SongbirdFilterModule {
        static_assert(std::is_floating_point<T>::value,
                "Must be provided with a floating point template type");

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
                                 _airGain(Parameters::AIR_GAIN.defaultValue),
                                 _outputGain(Parameters::OUTPUTGAIN.defaultValue),
                                 _modMode(Parameters::MODMODE_DEFAULT) {

            // initialise the filters to some default values
            setVowel1(_vowel1);
            setVowel2(_vowel2);

            // Set up the air filters
            const std::array<Formant, NUM_AIR_FORMANTS> airFormants {
                Formant(2700, -15), Formant(3500, -25)
            };

            _filtersAir[Channels::LEFT].setFormants(airFormants);
            _filtersAir[Channels::RIGHT].setFormants(airFormants);
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
         * Sets the output gain.
         *
         * @param[in]   val The output gain that should be used
         *
         * @see         OUTPUTGAIN for valid values
         */
        void setOutputGain(double val) { _outputGain = Parameters::OUTPUTGAIN.BoundsCheck(val); }

        /**
         * Sets the modulation mode to apply to the filters.
         *
         * @param[in]   val Chooses the modulation mode
         */
        void setModMode(bool val) { _modMode = val; }

        /**
         * Sets the level of the upper formant filter peaks.
         *
         * @param[in]   val Gain value that should be used
         *
         * @see         AIR_GAIN for valid values
         */
        void setAirGain(double val) { _airGain = Parameters::AIR_GAIN.BoundsCheck(val); }

        /**
         * Sets the modulation source that will be used to modulate the filter position.
         *
         * @param[in]   val The modulation source to use
         */
        void setModulationSource(std::shared_ptr<ModulationSource<double>> val) { _modulationSource = val; }

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
        int getVowel1() const { return _vowel1; }

        /**
         * @see setVowel2
         */
        int getVowel2() const { return _vowel2; }

        /**
         * Return a vowel object describing one of the built in vowels.
         *
         * @see     VowelParameter for valid values
         */
        inline Vowel getVowelDescription(int val) const;

        /**
         * @see setFilterPosition
         */
        double getFilterPosition() const { return _filterPosition; }

        /**
         * @see setMix
         */
        double getMix() const { return _mix; }

        /**
         * @see modMode
         */
        bool getModMode() const { return _modMode; }

        /**
         * @see setAirGain
         */
        double getAirGain() const { return _airGain; }

        /**
         * @see setOutputGain
         */
        double getOutputGain() const { return _outputGain; }

        /** @} */

        /**
         * Applies the filtering to a mono buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         *
         * @param   inSamples      Pointer to the first sample of the buffer
         * @param   numSamples     Number of samples in the buffer. The left and right buffers
         *                         must be the same size.
         */
        inline void Process1in1out(T* inSamples, size_t numSamples);

        /**
         * Applies the filtering to a stereo buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param   leftSamples    Pointer to the first sample of the left channel's buffer
         * @param   rightSamples   Pointer to the first sample of the right channel's buffer
         * @param   numSamples     Number of samples in the buffer. The left and right buffers
         *                         must be the same size.
         */
        inline void Process2in2out(T* leftSamples, T* rightSamples, size_t numSamples);

        SongbirdFilterModule operator=(SongbirdFilterModule& other) = delete;
        SongbirdFilterModule(SongbirdFilterModule& other) = delete;

    private:
        int _vowel1,
            _vowel2;

        double  _filterPosition,
                _sampleRate,
                _mix,
                _airGain,
                _outputGain;

        bool _modMode;

        std::shared_ptr<ModulationSource<double>> _modulationSource;

        std::map<Channels, SongbirdFormantFilter<T, NUM_FORMANTS_PER_VOWEL>> _filters1;
        std::map<Channels, SongbirdFormantFilter<T, NUM_FORMANTS_PER_VOWEL>> _filters2;
        std::map<Channels, SongbirdFormantFilter<T, NUM_AIR_FORMANTS>> _filtersAir;

        static constexpr unsigned int INTERNAL_BUFFER_SIZE = 512;

        T _leftOutputBuffer1[INTERNAL_BUFFER_SIZE];
        T _rightOutputBuffer1[INTERNAL_BUFFER_SIZE];
        T _leftOutputBuffer2[INTERNAL_BUFFER_SIZE];
        T _rightOutputBuffer2[INTERNAL_BUFFER_SIZE];
        T _leftOutputBufferAir[INTERNAL_BUFFER_SIZE];
        T _rightOutputBufferAir[INTERNAL_BUFFER_SIZE];

        /**
         * Sets the vowel sound that should be created by filter 1 using a Vowel object provided by
         * the caller rather than one of the built in Vowel objects stored in this class.
         *
         * @param   val Value that should be used for Vowel 1
         */
        inline void _setVowel1(const Vowel& val);

        /**
         * Uses the filterPosition parameter and the modulation source to calculate the vowel that
         * should be used when in MODMODE_FREQ, as this vowel will sit somewhere between the two
         * vowels that have been selected by the user.
         *
         * @param   modAmount Modulation amount to be applied
         */
        inline Vowel _calcVowelForFreqMode(double modAmount);

        /**
         * An array which defines all the formants that will be needed.
         */
        // (TODO: could be made static again)
        const Vowel _allFormants[NUM_VOWELS] {
            {Formant(800, 0), Formant(1150, -4)},
            {Formant(400, 0), Formant(2100, -24)},
            {Formant(350, 0), Formant(2400, -20)},
            {Formant(450, 0), Formant(800, -9)},
            {Formant(325, 0), Formant(700, -12)},
        };
    };

    template <typename T>
    void SongbirdFilterModule<T>::setVowel1(int val) {
        // perform a bounds check, then apply the appropriate formants
        _vowel1 = Parameters::VOWEL.BoundsCheck(val);

        _filters1[Channels::LEFT].setFormants(_allFormants[_vowel1 - 1]);
        _filters1[Channels::RIGHT].setFormants(_allFormants[_vowel1 - 1]);
    }

    template <typename T>
    void SongbirdFilterModule<T>::setVowel2(int val) {
        // perform a bounds check, then apply the appropriate formants
        _vowel2 = Parameters::VOWEL.BoundsCheck(val);

        _filters2[Channels::LEFT].setFormants(_allFormants[_vowel2 - 1]);
        _filters2[Channels::RIGHT].setFormants(_allFormants[_vowel2 - 1]);
    }

    template <typename T>
    void SongbirdFilterModule<T>::setSampleRate(double val) {
        _sampleRate = val;

        _filters1[Channels::LEFT].setSampleRate(val);
        _filters1[Channels::RIGHT].setSampleRate(val);
        _filters2[Channels::LEFT].setSampleRate(val);
        _filters2[Channels::RIGHT].setSampleRate(val);
        _filtersAir[Channels::LEFT].setSampleRate(val);
        _filtersAir[Channels::RIGHT].setSampleRate(val);
    }

    template <typename T>
    void SongbirdFilterModule<T>::reset() {
        _filters1[Channels::LEFT].reset();
        _filters1[Channels::RIGHT].reset();
        _filters2[Channels::LEFT].reset();
        _filters2[Channels::RIGHT].reset();
        _filtersAir[Channels::LEFT].reset();
        _filtersAir[Channels::RIGHT].reset();
    }

    template <typename T>
    Vowel SongbirdFilterModule<T>::getVowelDescription(int val) const {
        Vowel tempVowel;

        std::copy(&_allFormants[val - 1][0],
                  &_allFormants[val - 1][NUM_FORMANTS_PER_VOWEL],
                  std::begin(tempVowel));

        return tempVowel;
    }

    template <typename T>
    void SongbirdFilterModule<T>::Process1in1out(T* inSamples,
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
            T* const bufferInputStart {&inSamples[bufferNumber * INTERNAL_BUFFER_SIZE]};

            // Copy the samples we need to process in this chunk into the internal buffers
            std::copy(bufferInputStart, &bufferInputStart[numSamplesToCopy], _leftOutputBuffer1);
            std::copy(bufferInputStart, &bufferInputStart[numSamplesToCopy], _leftOutputBuffer2);
            std::copy(bufferInputStart, &bufferInputStart[numSamplesToCopy], _leftOutputBufferAir);

            // Figure out the modulation here. We have two ways to modulation between
            // two formant filters.
            // For MODMODE_BLEND we modulation the filter position to blend between the two filters.
            // For MODMOD_FREQ we set the filter position to 0 so that we're only using filter 1,
            // and then modulate the freqency of filter 1 between the two vowels
            double blendFilterPosition {0};
            const double modAmount {_modulationSource != nullptr ? _modulationSource->getNextOutput(bufferInputStart[0]) : 0};
            if (_modMode == Parameters::MODMODE_BLEND) {
                blendFilterPosition = _filterPosition + modAmount;
            } else {
                blendFilterPosition = 0;
                _setVowel1(_calcVowelForFreqMode(modAmount));
            }

            // Do the processing for each filter
            _filters1[Channels::LEFT].process(_leftOutputBuffer1, numSamplesToCopy);
            _filters2[Channels::LEFT].process(_leftOutputBuffer2, numSamplesToCopy);
            _filtersAir[Channels::LEFT].process(_leftOutputBufferAir, numSamplesToCopy);

            // Write to output, applying filter position and mix level
            // always use modFilterPosition as this will take into account any modulation
            for (size_t iii {0}; iii < numSamplesToCopy; iii++) {

                // Manually advance the modulation state through the rest of this buffer
                if (_modulationSource != nullptr && iii != 0) {
                    _modulationSource->getNextOutput(bufferInputStart[iii]);
                }

                bufferInputStart[iii] = (
                                            bufferInputStart[iii] * (1 - _mix)
                                            + _leftOutputBuffer1[iii] * (1 - blendFilterPosition) * _mix
                                            + _leftOutputBuffer2[iii] * blendFilterPosition * _mix
                                            + _leftOutputBufferAir[iii] * _airGain * _mix
                                        )
                                        * _outputGain;
            }
        }
    }

    template <typename T>
    void SongbirdFilterModule<T>::Process2in2out(T* leftSamples,
                                                 T* rightSamples,
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
            T* const leftBufferInputStart {&leftSamples[bufferNumber * INTERNAL_BUFFER_SIZE]};
            T* const rightBufferInputStart {&rightSamples[bufferNumber * INTERNAL_BUFFER_SIZE]};

            // Copy the samples we need to process in this chunk into the internal buffers
            std::copy(leftBufferInputStart, &leftBufferInputStart[numSamplesToCopy], _leftOutputBuffer1);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _rightOutputBuffer1);

            std::copy(leftBufferInputStart, &leftBufferInputStart[numSamplesToCopy], _leftOutputBuffer2);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _rightOutputBuffer2);

            std::copy(leftBufferInputStart, &leftBufferInputStart[numSamplesToCopy], _leftOutputBufferAir);
            std::copy(rightBufferInputStart, &rightBufferInputStart[numSamplesToCopy], _rightOutputBufferAir);

            // Figure out the modulation here. We have two ways to modulation between
            // two formant filters.
            // For MODMODE_BLEND we modulation the filter position to blend between the two filters.
            // For MODMOD_FREQ we set the filter position to 0 so that we're only using filter 1,
            // and then modulate the freqency of filter 1 between the two vowels
            double blendFilterPosition {0};
            const double modAmount {_modulationSource != nullptr ? _modulationSource->getNextOutput((leftBufferInputStart[0] + rightBufferInputStart[0]) / 2) : 0};
            if (_modMode == Parameters::MODMODE_BLEND) {
                blendFilterPosition = _filterPosition + modAmount;
            } else {
                blendFilterPosition = 0;
                _setVowel1(_calcVowelForFreqMode(modAmount));
            }

            // Do the processing for each filter
            _filters1[Channels::LEFT].process(_leftOutputBuffer1, numSamplesToCopy);
            _filters1[Channels::RIGHT].process(_rightOutputBuffer1, numSamplesToCopy);

            _filters2[Channels::LEFT].process(_leftOutputBuffer2, numSamplesToCopy);
            _filters2[Channels::RIGHT].process(_rightOutputBuffer2, numSamplesToCopy);

            _filtersAir[Channels::LEFT].process(_leftOutputBufferAir, numSamplesToCopy);
            _filtersAir[Channels::RIGHT].process(_rightOutputBufferAir, numSamplesToCopy);

            // Write to output, applying filter position and mix level
            // always use modFilterPosition as this will take into account any modulation
            for (size_t iii {0}; iii < numSamplesToCopy; iii++) {

                // Manually advance the modulation state through the rest of this buffer
                if (_modulationSource != nullptr && iii != 0) {
                    _modulationSource->getNextOutput((leftBufferInputStart[iii] + rightBufferInputStart[iii]) / 2);
                }

                leftBufferInputStart[iii] = (
                                                leftBufferInputStart[iii] * (1 - _mix)
                                                + _leftOutputBuffer1[iii] * (1 - blendFilterPosition) * _mix
                                                + _leftOutputBuffer2[iii] * blendFilterPosition * _mix
                                                + _leftOutputBufferAir[iii] * _airGain * _mix
                                            )
                                            * _outputGain;

                rightBufferInputStart[iii] = (
                                                rightBufferInputStart[iii] * (1 - _mix)
                                                + _rightOutputBuffer1[iii] * (1 - blendFilterPosition) * _mix
                                                + _rightOutputBuffer2[iii] * blendFilterPosition * _mix
                                                + _rightOutputBufferAir[iii] * _airGain * _mix
                                            )
                                            * _outputGain;
            }
        }
    }

    template <typename T>
    void SongbirdFilterModule<T>::_setVowel1(const Vowel& val) {
        _filters1[Channels::LEFT].setFormants(val);
        _filters1[Channels::RIGHT].setFormants(val);
    }

    template <typename T>
    Vowel SongbirdFilterModule<T>::_calcVowelForFreqMode(double modAmount) {
        // get the first and second vowels
        Vowel tempVowel1(getVowelDescription(getVowel1()));
        Vowel tempVowel2(getVowelDescription(getVowel2()));


        Vowel retVal(tempVowel1);

        for (size_t iii {0}; iii < NUM_FORMANTS_PER_VOWEL; iii++) {
            // TODO: There's definitely some optimisation to be done below

            // Calculate frequency modulation
            const double freqDelta {tempVowel2[iii].frequency - tempVowel1[iii].frequency};
            retVal[iii].frequency = tempVowel1[iii].frequency + freqDelta * (_filterPosition + modAmount);

            // Calculate gain modulation
            const double gainDelta {tempVowel2[iii].gaindB - tempVowel1[iii].gaindB};
            retVal[iii].gaindB = tempVowel1[iii].gaindB + gainDelta * (_filterPosition + modAmount);
        }

        return retVal;
    }
}
