/*
 *	File:		TooltipLabelUpdater.h
 *
 *	Version:	1.0.0
 *
 *	Created:	06/06/2021
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

namespace WECore::JUCEPlugin {

    /**
     * Acts as a MouseListener for multiple components, setting the given Label to display their
     * tooltip.
     */
    class TooltipLabelUpdater : public juce::MouseListener {
    public:
        inline TooltipLabelUpdater();
        ~TooltipLabelUpdater() = default;

        /**
         * Starts updating the label as necessary.
         */
        void start(juce::Label* targetLabel) { _targetLabel = targetLabel; }

        /**
         * Must be called before the given label is destructed.
         */
        void stop() { _targetLabel = nullptr; }

        inline virtual void mouseEnter(const juce::MouseEvent &event) override;
        inline virtual void mouseExit(const juce::MouseEvent &event) override;

    private:
        juce::Label* _targetLabel;
    };

    TooltipLabelUpdater::TooltipLabelUpdater() : _targetLabel(nullptr) {
    }

    void TooltipLabelUpdater::mouseEnter(const juce::MouseEvent &event) {
        if (_targetLabel != nullptr) {
            juce::Component* component = juce::Desktop::getInstance().getMainMouseSource().getComponentUnderMouse();

            TooltipClient* tooltipClient = dynamic_cast<TooltipClient*>(component);

            if (tooltipClient != nullptr) {
                _targetLabel->setText(tooltipClient->getTooltip(), dontSendNotification);
            }
        }
    }

    void TooltipLabelUpdater::mouseExit(const juce::MouseEvent &event) {
        if (_targetLabel != nullptr) {
            _targetLabel->setText("", dontSendNotification);
        }
    }
}
