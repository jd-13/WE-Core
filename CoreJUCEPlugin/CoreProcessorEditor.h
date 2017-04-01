/*
 *	File:		CoreProcessorEditor.h
 *
 *	Version:	1.0.0
 *
 *	Created:	18/03/2017
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

#ifndef COREPROCESSOREDITOR_H_INCLUDED
#define COREPROCESSOREDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * This class provides basic functionality that is commonly used by an AudioProcessorEditor in a
 * White Elephant plugin.
 */
class CoreProcessorEditor : public AudioProcessorEditor {
public:
    
    CoreProcessorEditor() = delete;
    
    ~CoreProcessorEditor() = default;
    
protected:
    CoreProcessorEditor(AudioProcessor& ownerFilter) : AudioProcessorEditor(ownerFilter) {}

    
    SharedResourcePointer<TooltipWindow> _tooltipWindow;
    
    /**
     * Sets the look and feel for all child components.
     *
     * Previously LookAndFeel::setDefaultLookAndFeel(&defaultLookAndFeel); was used but this
     * resulted in a bug where upon opening two instances of the same plugin simultaneously, when
     * one is closed the other will stop applying defaultLookAndFeel.
     */
    void _assignLookAndFeelToAllChildren(LookAndFeel& defaultLookAndFeel) {
        _tooltipWindow->setLookAndFeel(&defaultLookAndFeel);
        for (int iii {0}; iii < getNumChildComponents(); iii++) {
            getChildComponent(iii)->setLookAndFeel(&defaultLookAndFeel);
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreProcessorEditor)
};


#endif  // COREPROCESSOREDITOR_H_INCLUDED
