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

#ifndef SONGBIRDFILTER_H_INCLUDED
#define SONGBIRDFILTER_H_INCLUDED

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
 * It is recommended to set all available parameters explicitly by calling their setter methods
 * before attempting to process any audio.
 *
 * Internally relies on the parameters provided in SongbirdFiltersParameters.h
 *
 * @see SongbirdFormantFilter   - SongbirdFilterModule is composed of two pairs of
 *                                SongbirdFormantFilters (pairs to allow stereo processing), each
 *                                pair is assigned one of the five supported vowels at any time
 */
class SongbirdFilterModule {
public:
    /**
     * Does some basic setup and defaulting of parameters, though do not rely on this
     * to sensibly default all parameters.
     */
    SongbirdFilterModule() :    vowel1(VOWEL.VOWEL_A),
                                vowel2(VOWEL.VOWEL_E),
                                filterPosition(FILTER_POSITION.defaultValue),
                                sampleRate(44100),
                                mix(MIX.defaultValue),
                                modulationSrc(MODULATION.defaultValue),
                                modMode(MODMODE_DEFAULT) {

        // initialise the filters to some default values
        setVowel1(vowel1);
        setVowel2(vowel2);
    }

    virtual ~SongbirdFilterModule() {}

    /**
     * Sets the vowel sound that should be created by filter 1 using one of
     * the built in Vowel objects stored in this class.
     *
     * @param   val Value that should be used for Vowel 1
     *
     * @see     VowelParameter for valid values
     */
    void setVowel1(int val) {
        // perform a bounds check, then apply the appropriate formants
        vowel1 = VOWEL.BoundsCheck(val);

        const std::vector<Formant> tempFormants(&allFormants[vowel1 - 1][0],
                                                &allFormants[vowel1 - 1][NUM_FORMANTS_PER_VOWEL]);

        filters1[Channels::LEFT].setFormants(tempFormants);
        filters1[Channels::RIGHT].setFormants(tempFormants);
    }

    /**
     * Sets the vowel sound that should be created by filter 2.
     *
     * @param   val Value that should be used for Vowel 2
     *
     * @see     VowelParameter for valid values
     */
    void setVowel2(int val) {
        // perform a bounds check, then apply the appropriate formants
        vowel2 = VOWEL.BoundsCheck(val);

        const std::vector<Formant> tempFormants(&allFormants[vowel2 - 1][0],
                                                &allFormants[vowel2 - 1][NUM_FORMANTS_PER_VOWEL]);
        filters2[Channels::LEFT].setFormants(tempFormants);
        filters2[Channels::RIGHT].setFormants(tempFormants);
    }

    /**
     * Sets the position between the two filters that have been selected.
     * The effect of this parameter is dependant on the value of modMode.
     * If MODMODE_BLEND is selected, then two sets of filters will be created
     *
     */
    void setFilterPosition(double val) { filterPosition = FILTER_POSITION.BoundsCheck(val); }

    /**
     * Set the sample rate that the filters expect of the audio which will be processed.
     *
     * @param   val The sample rate to set the filters to
     */
    void setSampleRate(double val) {
        sampleRate = val;

        filters1[Channels::LEFT].setSampleRate(val);
        filters1[Channels::RIGHT].setSampleRate(val);
        filters2[Channels::LEFT].setSampleRate(val);
        filters2[Channels::RIGHT].setSampleRate(val);
    }

    /**
     * Sets the dry/wet mix level.
     * Lowest value = completely dry, unprocessed signal, no filtering applied.
     * Highest value = completely wet signal, no unprocessed audio survives.
     *
     * @param   val Mix value that should be used
     *
     * @see     MIX for valid values
     */
    void setMix(double val) { mix = MIX.BoundsCheck(val); }

    void setModulation(float val) { modulationSrc = MODULATION.BoundsCheck(val); }

    void setModMode(bool val) { modMode = val; }

    /**
     * Resets all filters.
     * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
     */
    void reset() {
        filters1[Channels::LEFT].reset();
        filters1[Channels::RIGHT].reset();
        filters2[Channels::LEFT].reset();
        filters2[Channels::RIGHT].reset();
    }

    /**
     * @see setVowel1
     */
    int getVowel1() { return vowel1; }

    /**
     * @see setVowel2
     */
    int getVowel2() { return vowel2; }

    /**
     * Return a vowel object describing one of the built in vowels.
     *
     * @see     VowelParameter for valid values
     */
    Vowel getVowelDescription(int val) {
        Vowel tempVowel;

        std::copy(&allFormants[val - 1][0],
                  &allFormants[val - 1][NUM_FORMANTS_PER_VOWEL],
                  std::begin(tempVowel));

        return tempVowel;
    }

    /**
     * @see setFilterPosition
     */
    double getFilterPosition() { return filterPosition; }

    /**
     * @see setMix
     */
    double getMix() { return mix; }

    bool getModMode() { return modMode; }

    /**
     * Applies the filtering to a stereo buffer of samples.
     * Expect seg faults or other memory issues if arguements passed are incorrect.
     *
     * @param   leftSamples    Pointer to the first sample of the left channel's buffer
     * @param   rightSamples   Pointer to the first sample of the right channel's buffer
     * @param   numSamples     Number of samples in the buffer. The left and right buffers
     *                         must be the same size.
     */
    void Process2in2out(double* leftSamples,
                        double* rightSamples,
                        size_t numSamples) {

        if (leftSamples != nullptr && rightSamples != nullptr && numSamples > 0) {

            // create two buffers of dry samples
            std::map<Channels, std::vector<double>> outputBuffer1;
            outputBuffer1[Channels::LEFT] = std::vector<double>(leftSamples,
                                                               leftSamples + numSamples);
            outputBuffer1[Channels::RIGHT] = std::vector<double>(rightSamples,
                                                                rightSamples + numSamples);

            std::map<Channels, std::vector<double>> outputBuffer2(outputBuffer1);


            // figure out the modulation here. We have two ways to modulation between
            // two formant filters. For MODMODE_BLEND we modulation the filter position
            // to blend between the two filters. For MODMOD_FREQ we set the filter
            // position to 0 so that we're only using filter 1, and then modulate the
            // freqency of filter 1 between the two vowels
            double blendFilterPosition;
            if (modMode == MODMODE_BLEND) {
                blendFilterPosition = filterPosition + modulationSrc;
            } else {
                blendFilterPosition = 0;
                setVowel1(calcVowelForFreqMode());
            }

            // do the processing for each filter
            filters1[Channels::LEFT].process(&outputBuffer1[Channels::LEFT][0], numSamples);
            filters1[Channels::RIGHT].process(&outputBuffer1[Channels::RIGHT][0], numSamples);

            filters2[Channels::LEFT].process(&outputBuffer2[Channels::LEFT][0], numSamples);
            filters2[Channels::RIGHT].process(&outputBuffer2[Channels::RIGHT][0], numSamples);

            // write to output, applying filter position and mix level
            // always use modFilterPosition as this will take into account any modulation
            for (size_t iii {0}; iii < numSamples; iii++) {
                leftSamples[iii] =  leftSamples[iii] * (1 - mix)
                + outputBuffer1[Channels::LEFT][iii] * (1 - blendFilterPosition) * mix
                + outputBuffer2[Channels::LEFT][iii] * blendFilterPosition * mix;

                rightSamples[iii] = rightSamples[iii] * (1 - mix)
                + outputBuffer1[Channels::RIGHT][iii] * (1 - blendFilterPosition) * mix
                + outputBuffer2[Channels::RIGHT][iii] * blendFilterPosition * mix;
            }
        }
    }

    SongbirdFilterModule operator=(SongbirdFilterModule& other) = delete;
    SongbirdFilterModule(SongbirdFilterModule& other) = delete;

private:
    int vowel1,
        vowel2;

    double  filterPosition,
            sampleRate,
            mix,
            modulationSrc;

    bool modMode;

    std::map<Channels, SongbirdFormantFilter> filters1;
    std::map<Channels, SongbirdFormantFilter> filters2;

    /**
     * Sets the vowel sound that should be created by filter 1 using a Vowel object provided by the
     * caller rather than one of the built in Vowel objects stored in this class.
     *
     * @param   val Value that should be used for Vowel 1
     */
    void setVowel1(Vowel val) {
        const std::vector<Formant> tempFormants(val.begin(), val.end());

        filters1[Channels::LEFT].setFormants(tempFormants);
        filters1[Channels::RIGHT].setFormants(tempFormants);
    }

    /**
     * Uses the filterPosition parameter and the modulation source to calculate the vowel that
     * should be used when in MODMODE_FREQ, as this vowel will sit somewhere between the two vowels
     * that have been selected by the user.
     */
    Vowel calcVowelForFreqMode() {
        // get the first and second vowels
        Vowel tempVowel1(getVowelDescription(getVowel1()));
        Vowel tempVowel2(getVowelDescription(getVowel2()));


        Vowel retVal(tempVowel1);

        for (size_t iii {0}; iii < NUM_FORMANTS_PER_VOWEL; iii++) {
            // TODO: There's definitely some optimisation to be done below

            // Calculate frequency modulation
            float freqDelta {std::fabs(tempVowel1[iii].frequency - tempVowel2[iii].frequency)};
            // Invert the delta depending on which value is largest
            freqDelta *= (tempVowel1[iii].frequency > tempVowel2[iii].frequency) ? -1.0 : 1.0;

            retVal[iii].frequency = tempVowel1[iii].frequency + freqDelta / 2;
            retVal[iii].frequency += (freqDelta / 2) * modulationSrc
                                     + (freqDelta / 2) * ((filterPosition - 0.5) * 2);

            // Calculate gain modulation
            float gainDelta {std::fabs(tempVowel1[iii].gaindB - tempVowel2[iii].gaindB)};
            // Invert the delta depending on which value is largest
            gainDelta *= (tempVowel1[iii].gaindB > tempVowel2[iii].gaindB) ? -1.0 : 1.0;

            retVal[iii].gaindB = tempVowel1[iii].gaindB + gainDelta / 2;
            retVal[iii].gaindB += (gainDelta / 2) * modulationSrc
                                  + (gainDelta / 2) * ((filterPosition - 0.5) * 2);
        }

        return retVal;
    }

    /**
     * An array which defines all the formants that will be needed.
     */
    // (TODO: could be made static again)
    const Formant allFormants[NUM_VOWELS][NUM_FORMANTS_PER_VOWEL] {
        {Formant(800, 0), Formant(1150, -4), Formant(2800, -20), Formant(3500, -36), Formant(4950, -60)},
        {Formant(400, 0), Formant(1600, -24), Formant(2700, -30), Formant(3300, -35), Formant(4950, -60)},
        {Formant(350, 0), Formant(1700, -20), Formant(2700, -30), Formant(3700, -36), Formant(4950, -60)},
        {Formant(450, 0), Formant(800, -9), Formant(2830, -16), Formant(3500, -28), Formant(4950, -55)},
        {Formant(325, 0), Formant(700, -12), Formant(2530, -30), Formant(3500, -40), Formant(4950, -64)},
    };
};

#endif  // SONGBIRDFILTER_H_INCLUDED
