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

enum	{kWaveArraySize = 2000};

class WaveParameter : public ParameterDefinition::BaseParameter<int> {
public:
    using ParameterDefinition::BaseParameter<int>::BaseParameter;
    
    static const int    SINE = 1,
                        SQUARE = 2,
                        SAW = 3;
};

const WaveParameter WAVE(1, 3, 1);

const ParameterDefinition::RangedParameter<int> TEMPONUMER(1, 4, 1),
                                                TEMPODENOM(1, 32, 1),
                                                PHASE(0, kWaveArraySize, 0);

const ParameterDefinition::RangedParameter<float>   DEPTH(0, 1, 0.5),
                                                    DEPTHMOD(0, 1, 0),
                                                    FREQ(0.5, 20, 2),
                                                    FREQMOD(0, 1, 0);

const bool  LFOSWITCH_OFF = false,
            LFOSWITCH_ON = true,
            LFOSWITCH_DEFAULT = LFOSWITCH_OFF,

            TEMPOSYNC_OFF = false,
            TEMPOSYNC_ON = true,
            TEMPOSYNC_DEFAULT = TEMPOSYNC_OFF,

            PHASESYNC_OFF = false,
            PHASESYNC_ON = true,
            PHASESYNC_DEFAULT = PHASESYNC_OFF;

#endif /* CarveParameters_h */