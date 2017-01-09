/*
 *  File:       SongbirdFormantFilter.h
 *
 *  Version:    1.0.0
 *
 *  Created:    16/07/2016
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

#ifndef SONGBIRDFORMANTFILTER_H_INCLUDED
#define SONGBIRDFORMANTFILTER_H_INCLUDED

#include "Formant.h"
#include "TPTSVFilter.h"
#include <vector>

/**
 * A class containing a vector of bandpass filters to produce a vowel sound.
 *
 * Supports only mono audio processing. For stereo processing, you must create
 * two objects of this type. (Do not reuse a single object for both channels)
 *
 * To use this class, simply call setFormants, reset, and process as necessary.
 *
 * @see setFormants - must be called before performing any processing
 * @see Formant     - Formant objects are required for operation of this class
 */
class SongbirdFormantFilter {
public:
    /**
     * Creates and stores the appropriate number of filters.
     *
     * @param   numFormants The number of formant peaks the filter should produce
     *                      (therefore the number of bandpass filters it will need to
     *                      contain), defaults to 5
     */
    SongbirdFormantFilter(int numFormants = 5) {
        for (int iii {0}; iii < numFormants; iii++) {
            TPTSVFilter* tempFilter = new TPTSVFilter();
            tempFilter->setMode(TPTSVFilterParameters::FILTER_MODE.PEAK);
            tempFilter->setQ(15);
            filters.push_back(tempFilter);
        }
    }
    
    /**
     * Deallocates each filter in the vector.
     */
    virtual ~SongbirdFormantFilter() {
        for (size_t iii {0}; iii < filters.size(); iii++) {
            TPTSVFilter* tempFilter {filters[iii]};
            delete tempFilter;
        }
    }
    
    /**
     * Applies the filtering to a mono buffer of samples.
     * Expect seg faults or other memory issues if arguements passed are incorrect.
     *
     * @param   inSamples   Pointer to the first sample of the buffer
     * @param   numSamples  Number of samples in the buffer
     */
    void process(double* inSamples, size_t numSamples) {
        if (numSamples > 0 && inSamples != nullptr) {
            
            // initialise the empty output buffer
            std::vector<double> outputBuffer(numSamples, 0);
            
            // perform the filtering for each formant peak
            for (size_t iii {0}; iii < filters.size(); iii++) {
                // copy the input samples to a new buffer
                std::vector<double> tempBuffer(inSamples, inSamples + numSamples);
                
                filters[iii]->processBlock(&tempBuffer[0], numSamples);
                
                // add the processed samples to the output buffer
                for (size_t jjj {0}; jjj < tempBuffer.size(); jjj++) {
                    outputBuffer[jjj] += tempBuffer[jjj];
                }
            }
            
            // write the buffer to output
            for (size_t iii {0}; iii < numSamples; iii++) {
                inSamples[iii] = outputBuffer[iii];
            }
        }
    }
    
    /**
     * Sets the properties of each bandpass filter contained in the object.
     *
     * @param   formants    A vector of Formants, the size of which must equal the 
     *                      number of bandpass filters in the object, as a single Formant
     *                      is applied to single bandpass filter in a one-to-one fashion
     *
     * @return  A boolean value, true if the formants have been applied to the filters
     *          correctly, false if the operation failed
     *
     * @see     Formant - This object is used to as a convenient container of all the
     *                    parameters which can be supplied to a bandpass filter.
     */
    bool setFormants(std::vector<Formant> formants) {
        bool retVal {false};
        
        // if the correct number of formants have been supplied,
        // apply them to each filter in turn
        if (filters.size() == formants.size()) {
            retVal = true;
            
            for (size_t iii {0}; iii < filters.size(); iii++) {
                filters[iii]->setCutoff(formants[iii].frequency);
                
                double gainAbs = pow(10, formants[iii].gaindB / 20.0);
                filters[iii]->setGain(gainAbs);
            }
        }
        
        return retVal;
    }
    
    /**
     * Sets the sample rate which the filters will be operating on.
     */
    void setSampleRate(double val) {
        for (TPTSVFilter* filter : filters) {
            filter->setSampleRate(val);
        }
    }
    
    /**
     * Resets all filters.
     * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
     */
    void reset() {
        for (TPTSVFilter* filter : filters) {
            filter->reset();
        }
    }
    
private:
    std::vector<TPTSVFilter*> filters;
};



#endif  // SONGBIRDFORMANTFILTER_H_INCLUDED
