/*
 *  File:       CoreTestLib.h
 *
 *  Version:    1.0.0
 *
 *  Created:    15/11/2016
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

#ifndef CoreTestLib_h
#define CoreTestLib_h

#include <iostream>

namespace CoreTestLib {
    const float TOLERANCE_6DP {0.000001f};
    const float TOLERANCE_4DP {0.0001f};

    
    template <typename T>
    bool compareFloats(T x, T y, T tolerance = std::numeric_limits<T>::epsilon()) {
        bool retVal {std::abs(x - y) < tolerance};
        
        if (!retVal) {
            std::cout << x << " != " << y << std::endl;
        }
        return retVal;
    }
}

#endif /* CoreTestLib_h */
