/*
 *  File:       SongbirdFiltersParameters.h
 *
 *  Version:    1.0.0
 *
 *  Created:    02/10/2016
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

#ifndef SongbirdFiltersParameters_h
#define SongbirdFiltersParameters_h

#include "General/ParameterDefinition.h"

class VowelParameter : public ParameterDefinition::BaseParameter<int> {
public:
    using ParameterDefinition::BaseParameter<int>::BaseParameter;
    
    static const int    VOWEL_A {1},
                        VOWEL_E {2},
                        VOWEL_I {3},
                        VOWEL_O {4},
                        VOWEL_U {5};
};

//@{
/**
 * A parameter which can take any float value between the ranges defined.
 * The values passed on construction are in the following order:
 *      minimum value,
 *      maximum value,
 *      default value
 */
const ParameterDefinition::RangedParameter<double>  FILTER_POSITION(0, 1, 0.5),
                                                    MIX(0, 1, 1),
                                                    MODULATION(-1, 1, 0);
//@}

const VowelParameter VOWEL(1, 5, 1);

const bool  MODMODE_BLEND = false,
            MODMODE_FREQ = true,
            MODMODE_DEFAULT = MODMODE_BLEND;

const int FILTER_ORDER {8};

#endif /* SongbirdFiltersParameters_h */
