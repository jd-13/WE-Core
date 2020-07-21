/*
 *	File:		LabelReadoutSlider.h
 *
 *	Version:	1.0.0
 *
 *	Created:	01/07/2020
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
#include "General/ParameterDefinition.h"

namespace WECore::JUCEPlugin {

    /**
     * Handles mouse events that may indicate that the Slider value has changed.
     */
    class SliderLabelUpdater : public Slider {
    public:
        explicit SliderLabelUpdater(const String& componentName) : Slider(componentName) {}
        virtual ~SliderLabelUpdater() = default;

        /** @name Mouse event handlers */
        /** @{ */
        virtual void mouseEnter(const MouseEvent& event) override {
            Slider::mouseEnter(event);
            _updateLabel();
        }

        virtual void mouseExit(const MouseEvent& event) override {
            Slider::mouseExit(event);
            _resetLabel();
        }

        virtual void mouseDoubleClick(const MouseEvent& event) override {
            Slider::mouseDoubleClick(event);
            _updateLabel();
        }

        virtual void mouseDrag(const MouseEvent& event) override {
            Slider::mouseDrag(event);
            _updateLabel();
        }

        virtual void mouseWheelMove(const MouseEvent& event,
                                    const MouseWheelDetails& wheel) override {
            Slider::mouseWheelMove(event, wheel);
            _updateLabel();
        }
        /** @} */

    private:
        /**
         * Called when the Slider value may have changed and the Label(s) should be updated.
         */
        virtual void _updateLabel() = 0;

        /**
         * Called when the mouse is no longer over the Slider, so the Label(s) can be reset.
         */
        virtual void _resetLabel() = 0;
    };

    /**
     * Outputs the value of the Slider to a label while hovering over the slider.
     */
    template <class T>
    class LabelReadoutSlider : public SliderLabelUpdater {
    public:
        explicit LabelReadoutSlider(const String& componentName) : SliderLabelUpdater(componentName),
                                                                   _targetLabel(nullptr),
                                                                   _parameter(nullptr),
                                                                   _isRunning(false) {}

        virtual ~LabelReadoutSlider() = default;

        /**
         * Tells the slider to start writing to the label on mouse enter events.
         *
         * Doesn't take ownership of the label or parameter.
         */
        inline void start(Label* targetLabel,
                          String labelText,
                          const ParameterDefinition::RangedParameter<T>* parameter);

        /**
         * Tells the slider to stop writing to the label.
         *
         * Call this in your destructor if the Label or RangedParameter might which we depend on
         * might be deallocated before LabelReadoutSlider.
         */
        inline void stop();

    private:
        Label* _targetLabel;
        String _labelText;
        const ParameterDefinition::RangedParameter<T>* _parameter;
        bool _isRunning;

        inline virtual void _updateLabel() override;

        inline virtual void _resetLabel() override;
    };

    template <class T>
    void LabelReadoutSlider<T>::start(Label* targetLabel,
                                    String labelText,
                                    const ParameterDefinition::RangedParameter<T>* parameter) {
        _targetLabel = targetLabel;
        _labelText = labelText;
        _parameter = parameter;
        _isRunning = true;
    }

    template <class T>
    void LabelReadoutSlider<T>::stop() {
        _isRunning = false;
    }

    template <class T>
    void LabelReadoutSlider<T>::_updateLabel() {
        if (_isRunning) {
            String valueString(_parameter->NormalisedToInternal(getValue()), 2);
            _targetLabel->setText(valueString, dontSendNotification);
        }
    }

    template <class T>
    void LabelReadoutSlider<T>::_resetLabel() {
        if (_isRunning) {
            _targetLabel->setText(_labelText, dontSendNotification);
        }
    }
}
