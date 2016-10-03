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

// a class containing a vector of bandpass filters 
class SongbirdFormantFilter {
public:
    SongbirdFormantFilter(int numFormants = 5) {
        // create and store the appropriate number of filters
        for (int iii {0}; iii < numFormants; iii++) {
            SongbirdBandPassFilter* tempFilter = new SongbirdBandPassFilter();
            filters.push_back(tempFilter);
        }
    }
    
    ~SongbirdFormantFilter() {
        for (size_t iii {0}; iii < filters.size(); iii++) {
            SongbirdBandPassFilter* tempFilter {filters[iii]};
            delete tempFilter;
        }
    }
    
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
    
    void reset() {
        for (SongbirdBandPassFilter* filter : filters) {
            filter->reset();
        }
    }
    
private:
    std::vector<SongbirdBandPassFilter*> filters;
};



#endif  // SONGBIRDFORMANTFILTER_H_INCLUDED
