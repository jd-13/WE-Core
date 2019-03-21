/*
 *	File:		PopupMenuV2.h
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

namespace WECore::LookAndFeelMixins {

    /**
     * V2 (December 2018) style lookandfeel popup ment mixin.
     *
     * Uses the following colours:
     * -# ** PopupMenu::highlightedBackgroundColourId **
     * -# ** PopupMenu::highlightedTextColourId **
     * -# ** PopupMenu::textColourId **
     */
    template <typename BASE>
    class PopupMenuV2 : public BASE {

    public:
        PopupMenuV2() : _popupMenuFontName("Courier New") {}
        virtual ~PopupMenuV2() = default;

        /** @{ LookAndFeel overrides */
        inline virtual void drawPopupMenuItem(Graphics& g,
                                              const Rectangle<int>& area,
                                              bool isSeparator,
                                              bool isActive,
                                              bool isHighlighted,
                                              bool isTicked,
                                              bool hasSubMenu,
                                              const String& text,
                                              const String& shortcutKeyText,
                                              const Drawable* icon,
                                              const Colour* textColour) override;

        inline virtual Font getPopupMenuFont() override;
        /** @} */

        void setPopupMenuFontName(const char* fontName) { _popupMenuFontName = fontName; }

    private:
        const char* _popupMenuFontName;
    };

    template <typename BASE>
    void PopupMenuV2<BASE>::drawPopupMenuItem(Graphics& g,
                                              const Rectangle<int>& area,
                                              bool /*isSeparator*/,
                                              bool /*isActive*/,
                                              bool isHighlighted,
                                              bool isTicked,
                                              bool /*hasSubMenu*/,
                                              const String& text,
                                              const String& /*shortcutKeyText*/,
                                              const Drawable* /*icon*/,
                                              const Colour* /*textColour*/) {

        Rectangle<int> r = area.reduced(1);

        if (isHighlighted) {
            g.setColour(BASE::findColour(PopupMenu::highlightedBackgroundColourId));
            g.fillRect(r);

            g.setColour(BASE::findColour(PopupMenu::highlightedTextColourId));
        } else if (isTicked) {
            g.setColour(BASE::findColour(PopupMenu::highlightedBackgroundColourId).withAlpha(0.2f));
            g.fillRect(r);

            g.setColour(BASE::findColour(PopupMenu::textColourId));
        } else {
            g.setColour(BASE::findColour(PopupMenu::textColourId));
        }

        Font font(getPopupMenuFont());

        const float maxFontHeight {area.getHeight() / 1.3f};

        if (font.getHeight() > maxFontHeight) {
            font.setHeight(maxFontHeight);
        }

        g.setFont(font);

        r.removeFromLeft(3);
        g.drawFittedText(text, r, Justification::centredLeft, 1);
    }

    template <typename BASE>
    Font PopupMenuV2<BASE>::getPopupMenuFont() {
        Font comboFont;
        comboFont.setTypefaceName(_popupMenuFontName);
        return comboFont;
    }
}
