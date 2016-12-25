/*
 *  File:       TPTSVFilter
 *
 *  Version:    1.0.0
 *
 *  Created:    22/12/2016
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

#ifndef TPTSVFILTER_H_INCLUDED
#define TPTSVFILTER_H_INCLUDED

#include <cmath>
#include "TPTSVFilterParameters.h"

/**
 * A state variable filter from a topology-preserving transform.
 *
 * Based on a talk given by Ivan Cohen: https://www.youtube.com/watch?v=esjHXGPyrhg
 */
class TPTSVFilter {
public:
    TPTSVFilter() : _sampleRate(44100),
                    _cutoffHz(CUTOFF.defaultValue),
                    _Q(Q.defaultValue),
                    _mode(FILTER_MODE.BYPASS),
                    _gain(GAIN.defaultValue),
                    _s1(0),
                    _s2(0) {}
    
    virtual ~TPTSVFilter() {}
    
    void processBlock(float* inSamples, int numSamples) {
        
        if (_mode != FILTER_MODE.BYPASS) {
            const float g {std::tanf(M_PI * _cutoffHz / _sampleRate)};
            const float h {1.0f / (1 + g / _Q + g * g)};
            
            for (int iii {0}; iii < numSamples; iii++) {
                const float sample {inSamples[iii]};
                
                const float yH {h * (sample - (1.0f / _Q + g) * _s1 - _s2)};
                
                const float yB {g * yH + _s1};
                _s1 = g * yH + yB;
                
                const float yL {g * yB + _s2};
                _s2 = g * yB + yL;
                
                switch (_mode) {
                    case FILTER_MODE.BANDPASS:
                        inSamples[iii] = yB * _gain;
                        break;
                        
                    case FILTER_MODE.HIGHPASS:
                        inSamples[iii] = yH * _gain;
                        break;
                        
                    default:
                        inSamples[iii] = yL * _gain;
                        break;
                }
            }
        }
    }
    
    void reset() {
        _s1 = 0;
        _s2 = 0;
    }
    
    int getMode() const {return _mode;}
    float getCutoff() const {return _cutoffHz;}
    float getQ() const {return _Q;}
    float getGain() const {return _gain;}
    
    void setMode(int val) {_mode = MODE.BoundsCheck(val);}
    void setCutoff(float val) {_cutoffHz = CUTOFF.BoundsCheck(val);}
    void setQ(float val) {_Q = Q.BoundsCheck(val);}
    void setGain(float val) {_gain = GAIN.BoundsCheck(val);}
    
private:
    float   _sampleRate,
            _cutoffHz,
            _Q,
            _gain,
            _s1,
            _s2;
    
    int _mode;
};



#endif  // TPTSVFILTER_H_INCLUDED
