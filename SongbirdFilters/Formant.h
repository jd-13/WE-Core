/*
 *  File:       Formant.h
 *
 *  Version:    1.0.0
 *
 *  Created:    16/07/2016
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
 */

#ifndef FORMANT_H_INCLUDED
#define FORMANT_H_INCLUDED

/**
 * Simple class to hold the data about an individual formant peak
 */
class Formant {
public:
    Formant(int newFreq,
            int newGaindB,
            int newBandWidth) : frequency(newFreq),
    gaindB(newGaindB),
    bandWidth(newBandWidth) {}
    
    int frequency;
    int gaindB;
    int bandWidth;
};

#endif  // FORMANT_H_INCLUDED
