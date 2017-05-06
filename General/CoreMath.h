/*
 *  File:       CoreMath.h
 *
 *  Version:    1.0.0
 *
 *  Created:    18/03/2017
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

#ifndef CoreMath_h
#define CoreMath_h

#include <cmath>

/**
 * Contains generic math related consts and functions.
 */
namespace CoreMath {
    /**
     * Used for portability since Visual Studio has a different implementation of math.h
     */
    constexpr long double LONG_PI {3.14159265358979323846264338327950288};
    constexpr double DOUBLE_PI {static_cast<double>(LONG_PI)};
    
    constexpr long double LONG_TAU {2 * LONG_PI};
    constexpr double DOUBLE_TAU {static_cast<double>(LONG_TAU)};
    
    constexpr long double LONG_E {2.71828182845904523536028747135266250};
    constexpr double DOUBLE_E {static_cast<double>(LONG_E)};
    
    template <typename T>
    bool compareFloatsEqual(T x, T y, T tolerance = std::numeric_limits<T>::epsilon()) {
        return std::abs(x - y) < tolerance;
    }
}

#endif /* CoreMath_h */
