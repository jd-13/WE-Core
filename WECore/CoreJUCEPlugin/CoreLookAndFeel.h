/*
 *	File:		CoreLookAndFeel.h
 *
 *	Version:	2.0.0
 *
 *	Created:	17/09/2015
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

namespace WECore::JUCEPlugin {

    /**
     * A class which contains most of the basic design elements which the white elephant audio plugins
     * have in common.
     *
     * Not all drawing methods are defined, and so methods inherited from LookAndFeel_V2 may be used.
     *
     * By default the three colours which are used are dark grey, light grey, and neon blue. These can
     * be changed using the provided setter methods.
     *
     * The colour members (highlightColour, etc) are being phased out as it doesn't fit well with
     * how colours are managed in JUCE.
     */
    class CoreLookAndFeel : public LookAndFeel_V2 {
    public:
        CoreLookAndFeel() : lightColour(200, 200, 200),
                            darkColour(107, 107, 107),
                            highlightColour(34, 252, 255),
                            _fontName("Courier New") {
            // TODO: hook these values into setHighlightColour etc
            setColour(PopupMenu::highlightedBackgroundColourId, darkColour);
            setColour(PopupMenu::backgroundColourId, lightColour);
        }

        virtual ~CoreLookAndFeel() = default;

        CoreLookAndFeel operator=(CoreLookAndFeel&) = delete;
        CoreLookAndFeel(CoreLookAndFeel&) = delete;

        virtual inline void drawRotarySlider(Graphics& g,
                                             int /*x*/,
                                             int /*y*/,
                                             int width,
                                             int height,
                                             float /*sliderPosProportional*/,
                                             float /*rotaryStartAngle*/,
                                             float /*rotaryEndAngle*/,
                                             Slider &slider) override;

        virtual inline void drawLinearSliderThumb(Graphics& g,
                                                  int x,
                                                  int y,
                                                  int width,
                                                  int height,
                                                  float sliderPos,
                                                  float /*minSliderPos*/,
                                                  float /*maxSliderPos*/,
                                                  const Slider::SliderStyle style,
                                                  Slider& slider) override;

        virtual inline void drawButtonBackground(Graphics& g,
                                                    Button& button,
                                                    const Colour& /*backgroundColour*/,
                                                    bool /*isMouseOverButton*/,
                                                    bool /*isButtonDown*/) override;

        virtual inline void drawButtonText(Graphics& g,
                                           TextButton& textButton,
                                           bool /*isMouseOverButton*/,
                                           bool /*isButtonDown*/) override;

        virtual inline void drawComboBox(Graphics& g,
                                         int /*width*/,
                                         int /*height*/,
                                         const bool /*isButtonDown*/,
                                         int buttonX,
                                         int buttonY,
                                         int buttonW,
                                         int buttonH,
                                         ComboBox& box) override;

        virtual inline void drawLinearSliderBackground(Graphics& g,
                                                       int x,
                                                       int y,
                                                       int width,
                                                       int height,
                                                       float /*sliderPos*/,
                                                       float /*minSliderPos*/,
                                                       float /*maxSliderPos*/,
                                                       const Slider::SliderStyle /*style*/,
                                                       Slider& slider) override;

        virtual inline void drawTooltip(Graphics& g,
                                        const String& text,
                                        int width,
                                        int height) override;

        virtual void setHighlightColour(Colour newColour) {
            setColour(TextButton::buttonOnColourId, newColour);
            setColour(TextButton::textColourOnId, newColour);

            highlightColour = newColour;
        }

        virtual void setLightColour(Colour newColour) {
            setColour(TextButton::buttonColourId, newColour);
            setColour(TextButton::textColourOffId, newColour);

            lightColour = newColour;
        }

        virtual void setDarkColour(Colour newColour) {
            darkColour = newColour;
        }

        const char* getDefaultFontName() { return _fontName; }
        void setDefaultFontName(const char* fontName) { _fontName = fontName; }

    protected:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreLookAndFeel)

        Colour  lightColour,
                darkColour,
                highlightColour;

        const char* _fontName;
    };

    void CoreLookAndFeel::drawRotarySlider(Graphics& g,
                                           int /*x*/,
                                           int /*y*/,
                                           int width,
                                           int height,
                                           float /*sliderPosProportional*/,
                                           float /*rotaryStartAngle*/,
                                           float /*rotaryEndAngle*/,
                                           Slider &slider) {

        // calculate useful constants
        const double rangeOfMotion {260 * (CoreMath::DOUBLE_PI / 180)};
        const double rotation {((slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum())) * rangeOfMotion - rangeOfMotion / 2};

        const int margin {1};
        const float diameter {static_cast<float>(height - margin * 2)};

        // draw centre circle
        Path p;
        g.setColour(darkColour);
        p.addEllipse(width / 2 - diameter / 2, height / 2 - diameter / 2, diameter, diameter);
        g.fillPath(p);

        // draw outer ring
        if (slider.isEnabled()) {
            g.setColour(highlightColour);
        } else {
            g.setColour(lightColour);
        }

        p.clear();

        const double gap {0.4};
        p.addCentredArc(width / 2, height / 2, diameter / 2, diameter / 2, rotation, gap, 2 * CoreMath::DOUBLE_PI - gap, true);

        g.strokePath(p, PathStrokeType(2.0f));
    }

    void CoreLookAndFeel::drawLinearSliderThumb(Graphics& g,
                                                int x,
                                                int y,
                                                int width,
                                                int height,
                                                float sliderPos,
                                                float /*minSliderPos*/,
                                                float /*maxSliderPos*/,
                                                const Slider::SliderStyle style,
                                                Slider& slider) {

        const float sliderRadius = static_cast<float>(getSliderThumbRadius(slider) - 2);

        Colour* ring;

        if (slider.isEnabled()) {
            ring = &highlightColour;
        } else {
            ring = &lightColour;
        }

        if (style == Slider::LinearHorizontal || style == Slider::LinearVertical)
        {
            float kx, ky;

            if (style == Slider::LinearVertical)
            {
                kx = x + width * 0.5f;
                ky = sliderPos;
            }
            else
            {
                kx = sliderPos;
                ky = y + height * 0.5f;
            }

            Path p;
            p.addEllipse(kx - sliderRadius, ky - sliderRadius, sliderRadius * 2, sliderRadius * 2);

            g.setColour(darkColour);
            g.fillPath(p);

            g.setColour(*ring);
            g.strokePath(p, PathStrokeType(2.0f));
        }

    }

    void CoreLookAndFeel::drawButtonBackground(Graphics& g,
                                               Button& button,
                                               const Colour& /*backgroundColour*/,
                                               bool /*isMouseOverButton*/,
                                               bool /*isButtonDown*/) {
        const int width {button.getWidth()};
        const int height {button.getHeight()};

        const float indent {2.0f};
        const int cornerSize {jmin (roundToInt(width * 0.4f),
                                    roundToInt(height * 0.4f))};

        Path p;
        PathStrokeType pStroke(1);
        Colour* bc {nullptr};




        if (button.isEnabled()) {
            if (button.getToggleState()) {
                bc = &highlightColour;
            } else {
                bc = &lightColour;
            }
        } else {
            bc = &darkColour;
        }

        p.addRoundedRectangle(indent, indent, width - 2 * indent, height - 2 * indent, static_cast<float>(cornerSize));


        g.setColour(*bc);
        g.strokePath(p, pStroke);
    }

    void CoreLookAndFeel::drawButtonText(Graphics& g,
                                         TextButton& textButton,
                                         bool /*isMouseOverButton*/,
                                         bool /*isButtonDown*/) {

        Colour* textColour {nullptr};

        if (textButton.isEnabled()) {
            if (textButton.getToggleState() || textButton.getWidth() < 24) {
                textColour = &highlightColour;
            } else {
                textColour = &lightColour;
            }
        } else {
            textColour = &darkColour;
        }

        g.setColour(*textColour);
        int margin {0};

        // differentiates between the small button on the tempo sync ratio and larger buttons
        if (textButton.getWidth() > 24) {
            margin = 5;
        }

        g.drawFittedText(textButton.getButtonText(), margin, 0, textButton.getWidth() - 2 * margin, textButton.getHeight(), Justification::centred, 0);
    }

    void CoreLookAndFeel::drawComboBox(Graphics& g,
                                       int /*width*/,
                                       int /*height*/,
                                       const bool /*isButtonDown*/,
                                       int buttonX,
                                       int buttonY,
                                       int buttonW,
                                       int buttonH,
                                       ComboBox& box) {

        g.fillAll(lightColour);
        g.setColour(darkColour);
        g.fillRect(buttonX, buttonY, buttonW, buttonH);

        const float arrowX {0.2f};
        const float arrowH {0.3f};

        if (box.isEnabled()) {
            Path p;
            p.addTriangle(buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
                          buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                          buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);

            p.addTriangle(buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.55f + arrowH),
                          buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
                          buttonX + buttonW * arrowX,          buttonY + buttonH * 0.55f);

            g.setColour(box.isPopupActive() ? highlightColour : lightColour);

            g.fillPath(p);
        }
    }

    void CoreLookAndFeel::drawLinearSliderBackground(Graphics& g,
                                                     int x,
                                                     int y,
                                                     int width,
                                                     int height,
                                                     float /*sliderPos*/,
                                                     float /*minSliderPos*/,
                                                     float /*maxSliderPos*/,
                                                     const Slider::SliderStyle /*style*/,
                                                     Slider& slider) {
        g.setColour(lightColour);

        if (slider.isHorizontal()) {
            g.fillRect(x, y + height / 2, width, 2);
        }
    }

    void CoreLookAndFeel::drawTooltip(Graphics& g,
                                      const String& text,
                                      int width,
                                      int height) {
        g.setColour(lightColour);
        g.fillRect(0, 0, width, height);

        g.setColour(darkColour);
        g.drawFittedText(text, 0, 0, width, height, Justification::centred, 3);
    }
}
