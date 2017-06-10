/*
 *	File:		CoreAudioProcessor.h
 *
 *	Version:	1.0.0
 *
 *	Created:	10/06/2017
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

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * This class provides basic functionality that is commonly used by an AudioProcessor in a
 * White Elephant plugin.
 */
class CoreAudioProcessor : public AudioProcessor {
public:
    CoreAudioProcessor() = default;
    virtual ~CoreAudioProcessor() = default;
    
    bool needsUIUpdate() { return _UIUpdateFlag; }
    void requestUIUpdate() { _UIUpdateFlag = true; }
    void clearUIUpdate() { _UIUpdateFlag = false; }
    
protected:
    bool _UIUpdateFlag;
    
    String floatVectorToString(const std::vector<float>& fData) const {
        String result {""};
        
        if (fData.size() < 1) {
            return result;
        }
        
        for (int iii {0}; iii < (fData.size() - 1); iii++) {
            result << String(fData[iii])<<",";
        }
        
        result << String(fData[fData.size() - 1]);
        
        return result;
    }
    
    int stringToFloatVector(const String sFloatCSV, std::vector<float>& fData, int maxNumFloat) const {
        StringArray tokenizer;
        int tokenCount {tokenizer.addTokens(sFloatCSV, ",","")};
        int resultCount {(maxNumFloat <= tokenCount) ? maxNumFloat : tokenCount};
        
        for (int iii {0}; iii < resultCount; iii++) {
            fData.push_back(tokenizer[iii].getFloatValue());
        }
        
        return ((tokenCount <= maxNumFloat) ? resultCount : -1);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreAudioProcessor)
};
