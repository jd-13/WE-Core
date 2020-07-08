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
     * Outputs the value of the Slider to a label while hovering over the slider.
     */
    template <class T>
    class LabelReadoutSlider : public Slider {
    public:
        explicit LabelReadoutSlider (const String& componentName) : Slider(componentName),
                                                                  _targetLabel(nullptr),
                                                                  _parameter(nullptr),
                                                                  _isRunning(false) {}

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

        /** @name Mouse event handlers */
        /** @{ */
        inline virtual void mouseEnter(const MouseEvent& event) override;

        inline virtual void mouseExit(const MouseEvent& event) override;

        inline virtual void mouseDoubleClick(const MouseEvent& event) override;

        inline virtual void mouseDrag(const MouseEvent& event) override;

        inline virtual void mouseWheelMove(const MouseEvent& event,
                                           const MouseWheelDetails& wheel) override;
        /** @} */


    private:
        Label* _targetLabel;
        String _labelText;
        ParameterDefinition::RangedParameter<T>* _parameter;
        bool _isRunning;

        inline void _updateLabel();

        inline void _resetLabel();
    };

    template <class T>
    void LabelReadoutSlider<T>::start(Label* targetLabel,
                                    String labelText,
                                    const ParameterDefinition::RangedParameter<T>* parameter) {
        _targetLabel = targetLabel;
        _labelText = labelText;
        _parameter = const_cast<ParameterDefinition::RangedParameter<T>*>(parameter);
        _isRunning = true;
    }

    template <class T>
    void LabelReadoutSlider<T>::stop() {
        _isRunning = false;
    }

    template <class T>
    void LabelReadoutSlider<T>::mouseEnter(const MouseEvent& event) {
        Slider::mouseEnter(event);
        _updateLabel();
    }

    template <class T>
    void LabelReadoutSlider<T>::mouseExit(const MouseEvent& event) {
        Slider::mouseExit(event);
        _resetLabel();
    }

    template <class T>
    void LabelReadoutSlider<T>::mouseDoubleClick(const MouseEvent& event) {
        Slider::mouseDoubleClick(event);
        _updateLabel();
    }

    template <class T>
    void LabelReadoutSlider<T>::mouseDrag(const MouseEvent& event) {
        Slider::mouseDrag(event);
        _updateLabel();
    }

    template <class T>
    void LabelReadoutSlider<T>::mouseWheelMove(const MouseEvent& event,
                                               const MouseWheelDetails& wheel) {
        Slider::mouseWheelMove(event, wheel);
        _updateLabel();
    }

    template <class T>
    void LabelReadoutSlider<T>::_updateLabel() {
        if (_isRunning) {
            String valueString(_parameter->NormalisedToInteral(getValue()), 2);
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
