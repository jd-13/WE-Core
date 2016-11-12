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

#ifndef CORELOOKANDFEEL_H_INCLUDED
#define CORELOOKANDFEEL_H_INCLUDED

#define _USE_MATH_DEFINES

#include "math.h"
#include "../JuceLibraryCode/JuceHeader.h"



class CoreLookAndFeel : public LookAndFeel_V2 {
public:
    CoreLookAndFeel() :    LookAndFeel_V2(),
                            lightColour(200, 200, 200),
                            darkColour(107, 107, 107),
                            highlightColour(34, 252, 255) {
        setColour(PopupMenu::highlightedBackgroundColourId, darkColour);
        setColour(PopupMenu::backgroundColourId, lightColour);
    }
    
    virtual void drawRotarySlider(Graphics& g,
                                  int /*x*/,
                                  int /*y*/,
                                  int width,
                                  int height,
                                  float /*sliderPosProportional*/,
                                  float /*rotaryStartAngle*/,
                                  float /*rotaryEndAngle*/,
                                  Slider &slider) override {
        // calculate useful constants
        const double rangeOfMotion {260 * (M_PI / 180)};
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
        p.addCentredArc(width / 2, height / 2, diameter / 2, diameter / 2, rotation, gap, 2 * M_PI - gap, true);
        
        g.strokePath(p, PathStrokeType(2.0f));
    }
    
    virtual void drawLinearSliderThumb(Graphics& g,
                                       int x,
                                       int y,
                                       int width,
                                       int height,
                                       float sliderPos,
                                       float /*minSliderPos*/,
                                       float /*maxSliderPos*/,
                                       const Slider::SliderStyle style,
                                       Slider& slider) override {
        
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
    
    virtual void drawButtonBackground(Graphics& g,
                                                  Button& button,
                                                  const Colour& /*backgroundColour*/,
                                                  bool /*isMouseOverButton*/,
                                                  bool /*isButtonDown*/) override {
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
    
    virtual void drawButtonText(Graphics& g,
                                TextButton& textButton,
                                bool /*isMouseOverButton*/,
                                bool /*isButtonDown*/) override {
        
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
    
    virtual void drawComboBox(Graphics& g,
                              int /*width*/,
                              int /*height*/,
                              const bool /*isButtonDown*/,
                              int buttonX,
                              int buttonY,
                              int buttonW,
                              int buttonH,
                              ComboBox& box) override {
        
        g.fillAll(lightColour);
        g.setColour(darkColour);
        g.fillRect(buttonX, buttonY, buttonW, buttonH);
        
        const float arrowX {0.2f};
        const float arrowH {0.3f};
        
        if (box.isEnabled()) {
            Path p;
            p.addTriangle(buttonX + buttonW * 0.5f,                 buttonY + buttonH * (0.45f - arrowH),
                          buttonX + buttonW * (1.0f - arrowX),      buttonY + buttonH * 0.45f,
                          buttonX + buttonW * arrowX,               buttonY + buttonH * 0.45f);
            
            p.addTriangle(buttonX + buttonW * 0.5f,                 buttonY + buttonH * (0.55f + arrowH),
                          buttonX + buttonW * (1.0f - arrowX),      buttonY + buttonH * 0.55f,
                          buttonX + buttonW * arrowX,               buttonY + buttonH * 0.55f);
            
            g.setColour(box.isPopupActive() ? highlightColour : lightColour);
            
            g.fillPath(p);
        }
    }
    
    
    virtual void drawTooltip(Graphics& g,
                             const String& text,
                             int width,
                             int height) override {
        g.setColour(lightColour);
        g.fillRect(0, 0, width, height);
        
        g.setColour(darkColour);
        g.drawFittedText(text, 0, 0, width, height, Justification::centred, 3);
    }
    
    void setHighlightColour(Colour newColour) {
        highlightColour = newColour;
    }
    
    void setLightColour(Colour newColour) {
        lightColour = newColour;
    }
    
    void setDarkColour(Colour newColour) {
        darkColour = newColour;
    }
    
protected:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreLookAndFeel)
    
    Colour  lightColour,
            darkColour,
            highlightColour;
    
};


#endif  // CoreLOOKANDFEEL_H_INCLUDED
