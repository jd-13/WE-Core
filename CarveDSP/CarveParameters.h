//
//  CarveParameters.h
//  Carve
//
//  Created by Jack Devlin on 24/09/2016.
//
//

#ifndef CarveParameters_h
#define CarveParameters_h

#include "ParameterDefinition.h"

class ModeParameter : public ParameterDefinition::BaseParameter<int> {
public:
    using ParameterDefinition::BaseParameter<int>::BaseParameter;
    
    static const int   SINE = 1,
    PARABOLIC_SOFT = 2,
    PARABOLIC_HARD = 3,
    ASYMMETRIC_SINE = 4,
    EXPONENT = 5,
    CLIPPER = 6;
};

const ParameterDefinition::RangedParameter<float>   PREGAIN(0, 2, 1),
                                                    POSTGAIN(0, 2, 0.5),
                                                    TWEAK(-1, 1, 0),
                                                    ROUTING(0, 1, 0), // 0 = SERIAL, 1 = PARALLEL
                                                    DRYLEVEL(0, 2, 0),
                                                    MASTERVOL(0, 2, 1);
const ModeParameter MODE(1, 6, 1);

#endif /* CarveParameters_h */