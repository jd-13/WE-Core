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

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CoreAudioProcessor.h"

namespace WECore::JUCEPlugin {

    /**
     * This class provides basic functionality that is commonly used by an AudioProcessorEditor in a
     * White Elephant plugin.
     *
     * Classes inheriting from this should:
     *   - Override _onParameterUpdate and call it in the constructor
     */
class CoreProcessorEditor : public juce::AudioProcessorEditor {
    public:
        ~CoreProcessorEditor() {
            dynamic_cast<CoreAudioProcessor&>(processor).
                removeParameterChangeListener(&_parameterListener);
        }

    protected:

        /**
         * Is notified when a parameter has changed and calls _onParameterUpdate.
         */
        class ParameterChangeListener : public juce::ChangeListener {
        public:
            ParameterChangeListener(CoreProcessorEditor* parent) : _parent(parent) {};
            virtual ~ParameterChangeListener() = default;

            virtual void changeListenerCallback(juce::ChangeBroadcaster* /*source*/) {
                _parent->_onParameterUpdate();
            }

        private:
            CoreProcessorEditor* _parent;
        };

        ParameterChangeListener _parameterListener;

        juce::SharedResourcePointer<juce::TooltipWindow> _tooltipWindow;

        CoreProcessorEditor(CoreAudioProcessor& ownerFilter)
                : AudioProcessorEditor(ownerFilter),
                  _parameterListener(this) {
            dynamic_cast<CoreAudioProcessor&>(processor).
                addParameterChangeListener(&_parameterListener);
        }

        /**
         * Sets the look and feel for all child components.
         *
         * Previously LookAndFeel::setDefaultLookAndFeel(&defaultLookAndFeel); was used but this
         * resulted in a bug where upon opening two instances of the same plugin simultaneously, when
         * one is closed the other will stop applying defaultLookAndFeel.
         */
        void _assignLookAndFeelToAllChildren(juce::LookAndFeel& defaultLookAndFeel) {
            _tooltipWindow->setLookAndFeel(&defaultLookAndFeel);
            for (int iii {0}; iii < getNumChildComponents(); iii++) {
                getChildComponent(iii)->setLookAndFeel(&defaultLookAndFeel);
            }
        }

        /**
         * Sets the LookAndFeel for all child components to nullptr.
         *
         * Must be called before the LookAndFeel is deallocated, normally this is in the derived
         * editor's destructor.
         */
        void _removeLookAndFeelFromAllChildren() {
            _tooltipWindow->setLookAndFeel(nullptr);

            for (int iii {0}; iii < getNumChildComponents(); iii++) {
                getChildComponent(iii)->setLookAndFeel(nullptr);
            }
        }

        /**
         * This will be called whenever a parameter has been updated.
         */
        virtual void _onParameterUpdate() = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreProcessorEditor)
    };
}
