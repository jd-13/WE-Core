//
//  CoreTestLib.h
//  CarveTest
//
//  Created by Jack Devlin on 15/11/2016.
//
//

#ifndef CoreTestLib_h
#define CoreTestLib_h

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
