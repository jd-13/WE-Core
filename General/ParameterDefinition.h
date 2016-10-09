/*
 *  File:       ParameterDefinition.h
 *
 *  Version:    1.0.0
 *
 *  Created:    22/09/2016
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

#ifndef ParameterDefinition_h
#define ParameterDefinition_h

#include <unordered_map>
#include <string>


/**
 * Contains classes that are used for defining parameters. Note that these are not
 * intended to define individual parameters (and as such they will not store the
 * current value of a parameter), but are intended to define some characteristics
 * of a given type of parameter, such as the values that are valid for it and
 * provide some methods for performing calculations relating to those characteristics.
 */
namespace ParameterDefinition {
    class BooleanParameter {
    public:
        BooleanParameter(bool newDefaultValue) :  defaultValue(newDefaultValue) {}
        
        bool defaultValue;
    };
    
    /**
     * Provides basic functionality that may be useful for building other parameters from.
     */
    template <class A_Type>
    class BaseParameter {
    public:
        BaseParameter(A_Type newMinValue,
                      A_Type newMaxValue,
                      A_Type newDefaultValue) : minValue(newMinValue),
                                                maxValue(newMaxValue),
                                                defaultValue(newDefaultValue) {}
        
        A_Type  minValue,
                maxValue,
                defaultValue;
        
        /**
         * If the given value is between the minimum and maximum values for this parameter,
         * then the value is returned unchanged. If the given value is outside the minimum
         * and maximum values for this parameter, the given value is clipped to this range
         * and then returned.
         *
         * @param   val    Value to clip to minumum and maximum values
         *
         * @return  Clipped value
         */
        A_Type BoundsCheck(A_Type val) const {
            if (val < minValue) val = minValue;
            if (val > maxValue) val = maxValue;
            
            return val;
        }
    };
    
    /**
     * Provides storage for minimum, maximum and default values for a  parameter
     * which can contain a continuous value (such as a slider), as well as methods to convert
     * between the normalised and internal ranges, and clip a value to the appropriate range.
     */
    template <class A_Type>
    class RangedParameter: public BaseParameter<A_Type> {
    public:
        using BaseParameter<A_Type>::BaseParameter;

        /**
         * Translates parameter values from the normalised (0 to 1) range as required
         * by VSTs to the range used internally for that parameter
         *
         * @param   val    Normalised value of the parameter
         *
         * @return  The value of the parameter in the internal range for that parameter
         */
        A_Type NormalisedToInteral(A_Type val) const {
            return val * (this->maxValue - this->minValue) + this->minValue;
        }
        
        /**
         * Translates parameter values from the range used internally for that
         * parameter, to the normalised range (0 to 1) as required by VSTs.
         *
         * @param   val    Value of the parameter in the internal range
         *
         * @return  The normalised value of the parameter
         */
        A_Type InteralToNormalised(A_Type val) const {
            return (val - this->minValue) / (this->maxValue - this->minValue);
        }
    };
}




#endif /* ParameterDefinition_h */
