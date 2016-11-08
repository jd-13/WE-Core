/*
 *  File:       MONSTRParameters.h
 *
 *  Version:    1.0.0
 *
 *  Created:    08/11/2016
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

#ifndef MONSTRParameters_h
#define MONSTRParameters_h

#include "ParameterDefinition.h"

const bool  BANDSWITCH_OFF {false},
            BANDSWITCH_ON {true},
            BANDSWITCH_DEFAULT {BANDSWITCH_ON};

//@{
/**
 * A parameter which can take any float value between the ranges defined.
 * The values passed on construction are in the following order:
 *      minimum value,
 *      maximum value,
 *      default value
 */
const ParameterDefinition::RangedParameter<float>   CROSSOVERLOWER(40, 500, 100),
                                                    CROSSOVERUPPER(3000, 19500, 5000),
                                                    WIDTH(0, 2, 1);
//@}

#endif /* MONSTRParameters_h */