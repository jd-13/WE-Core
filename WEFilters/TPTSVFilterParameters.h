/*
 *  File:       TPTSVFilterParameters.h
 *
 *  Version:    1.0.0
 *
 *  Created:    25/12/2016
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

#ifndef TPTSVFILTERParameters_h
#define TPTSVFILTERParameters_h

#include "ParameterDefinition.h"

namespace TPTSVFilterParameters {
    class FilterModeParameter : public ParameterDefinition::BaseParameter<int> {
    public:
        using ParameterDefinition::BaseParameter<int>::BaseParameter;
        
        static const int    BYPASS = 1,
                            LOWPASS = 2,
                            PEAK = 3,
                            HIGHPASS = 4;
    };

    //@{
    /**
     * A parameter which can take any float value between the ranges defined.
     * The values passed on construction are in the following order:
     *      minimum value,
     *      maximum value,
     *      default value
     */
    const ParameterDefinition::RangedParameter<double>  CUTOFF(20, 20000, 20),
                                                        Q(0.1, 20, 0.5),
                                                        GAIN(0, 2, 1);
    //@}

    const FilterModeParameter FILTER_MODE(1, 4, 1);
}

#endif /* TPTSVFILTERParameters_h */