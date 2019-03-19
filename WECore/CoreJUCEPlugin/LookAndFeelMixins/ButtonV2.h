/*
 *	File:		ButtonV2.h
 *
 *	Version:	1.0.0
 *
 *	Created:	19/03/2019
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
#include "General/CoreMath.h"

namespace WECore::LookAndFeelMixins {

    /**
     * V2 (December 2018) style lookandfeel button mixin.
     */
    template <typename BASE>
    class ButtonV2 : public BASE {

    public:
        ButtonV2() : _buttonFontName("Courier New") {}
        virtual ~ButtonV2() = default;

        /** @{ LookAndFeel overrides */
        inline virtual void drawButtonBackground(Graphics& g,
                                                 Button& button,
                                                 const Colour& backgroundColour,
                                                 bool isMouseOverButton,
                                                 bool isButtonDown) override;

        inline virtual void drawButtonText(Graphics& g,
                                           TextButton& textButton,
                                           bool isMouseOverButton,
                                           bool isButtonDown) override;
        /** @} */

        void setButtonFontName(const char* fontName) { _buttonFontName = fontName; }

    private:
        static constexpr float _disabledDarker {0.7f};
        const char* _buttonFontName;
    };

    template <typename BASE>
    void ButtonV2<BASE>::drawButtonBackground(Graphics& g,
                                              Button& button,
                                              const Colour& /*backgroundColour*/,
                                              bool /*isMouseOverButton*/,
                                              bool /*isButtonDown*/) {
        const int width {button.getWidth()};
        const int height {button.getHeight()};

        constexpr float indent {2.0f};
        const int cornerSize {jmin (roundToInt(width * 0.4f),
                                    roundToInt(height * 0.4f))};

        Path p;
        PathStrokeType pStroke(1);

        if (button.isEnabled()) {
            if (button.getToggleState()) {
                g.setColour(button.findColour(TextButton::buttonOnColourId));
            } else {
                g.setColour(button.findColour(TextButton::buttonColourId));
            }
        } else {
            g.setColour(button.findColour(TextButton::buttonColourId).darker(_disabledDarker));
        }

        p.addRoundedRectangle(indent,
                              indent,
                              width - 2 * indent,
                              height - 2 * indent,
                              static_cast<float>(cornerSize));

        g.strokePath(p, pStroke);
    }

    template <typename BASE>
    void ButtonV2<BASE>::drawButtonText(Graphics& g,
                                        TextButton& textButton,
                                        bool /*isMouseOverButton*/,
                                        bool /*isButtonDown*/) {
        if (textButton.isEnabled()) {
            if (textButton.getToggleState()) {
                g.setColour(textButton.findColour(TextButton::textColourOnId));
            } else {
                g.setColour(textButton.findColour(TextButton::textColourOffId));
            }
        } else {
            g.setColour(textButton.findColour(TextButton::textColourOffId).darker(_disabledDarker));
        }

        constexpr int MARGIN {0};

        Font font;
        font.setTypefaceName(_buttonFontName);
        g.setFont(font);

        g.drawFittedText(textButton.getButtonText(),
                        MARGIN,
                        0,
                        textButton.getWidth() - 2 * MARGIN,
                        textButton.getHeight(),
                        Justification::centred,
                        0);
    }
}
