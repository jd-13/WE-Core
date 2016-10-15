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
#include "SongbirdBandPassFilter.h"

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
            SongbirdBandPassFilter* tempFilter = new SongbirdBandPassFilter();
            filters.push_back(tempFilter);
        }
    }
    
    /**
     * Deallocates each filter in the vector.
     */
    ~SongbirdFormantFilter() {
        for (size_t iii {0}; iii < filters.size(); iii++) {
            SongbirdBandPassFilter* tempFilter {filters[iii]};
            delete tempFilter;
        }
    }
    
    /**
     * Applies the filtering to a mono buffer of samples.
     * Expect seg faults or other memory issues if arguements passed are incorrect.
     *
     * @param   inSample    Pointer to the first sample of the buffer
     * @param   numSamples  Number of samples in the buffer
     */
    void process(float* inSamples, int numSamples) {
        if (numSamples > 0 && inSamples != nullptr) {
            
            // initialise the empty output buffer
            std::vector<float> outputBuffer(numSamples, 0);
            
            // perform the filtering for each formant peak
            for (size_t iii {0}; iii < filters.size(); iii++) {
                // copy the input samples to a new buffer
                std::vector<float> tempBuffer(inSamples, inSamples + numSamples);
                
                filters[iii]->process(&tempBuffer[0], numSamples);
                
                // add the processed samples to the output buffer
                for (size_t jjj {0}; jjj < tempBuffer.size(); jjj++) {
                    outputBuffer[jjj] += tempBuffer[jjj];
                }
            }
            
            // write the buffer to output
            for (int iii {0}; iii < numSamples; iii++) {
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
     * @param   sampleRate  The sample rate of the audio to be processed
     *
     * @return  A boolean value, true if the formants have been applied to the filters
     *          correctly, false if the operation failed
     *
     * @see     Formant - This object is used to as a convenient container of all the
     *                    parameters which can be supplied to a bandpass filter.
     */
    bool setFormants(std::vector<Formant> formants, double sampleRate) {
        bool retVal {false};
        
        // if the correct number of formants have been supplied,
        // apply them to each filter in turn
        if (filters.size() == formants.size()) {
            retVal = true;
            
            for (size_t iii {0}; iii < filters.size(); iii++) {
                filters[iii]->setup(sampleRate,
                                    formants[iii].frequency,
                                    formants[iii].bandWidth,
                                    formants[iii].gaindB);
            }
        }
        
        return retVal;
    }
    
    /**
     * Resets all filters.
     * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
     */
    void reset() {
        for (SongbirdBandPassFilter* filter : filters) {
            filter->reset();
        }
    }
    
private:
    std::vector<SongbirdBandPassFilter*> filters;
};



#endif  // SONGBIRDFORMANTFILTER_H_INCLUDED