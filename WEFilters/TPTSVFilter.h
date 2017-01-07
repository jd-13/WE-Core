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
                    _cutoffHz(TPTSVFilterParameters::CUTOFF.defaultValue),
                    _Q(TPTSVFilterParameters::Q.defaultValue),
                    _gain(TPTSVFilterParameters::GAIN.defaultValue),
                    _s1(0),
                    _s2(0),
                    _mode(TPTSVFilterParameters::FILTER_MODE.BYPASS) {}
    
    virtual ~TPTSVFilter() {}
    
    void processBlock(double* inSamples, size_t numSamples) {
        
        if (_mode != TPTSVFilterParameters::FILTER_MODE.BYPASS) {
            const double g {std::tan(M_PI * _cutoffHz / _sampleRate)};
            const double h {1.0f / (1 + g / _Q + g * g)};
            
            for (size_t iii {0}; iii < numSamples; iii++) {
                const double sample {inSamples[iii]};
                
                const double yH {h * (sample - (1.0f / _Q + g) * _s1 - _s2)};
                
                const double yB {g * yH + _s1};
                _s1 = g * yH + yB;
                
                const double yL {g * yB + _s2};
                _s2 = g * yB + yL;
                
                switch (_mode) {
                    case TPTSVFilterParameters::FILTER_MODE.PEAK:
                        inSamples[iii] = yB * _gain;
                        break;
                        
                    case TPTSVFilterParameters::FILTER_MODE.HIGHPASS:
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
    double getCutoff() const {return _cutoffHz;}
    double getQ() const {return _Q;}
    double getGain() const {return _gain;}
    
    void setMode(int val) {_mode = TPTSVFilterParameters::FILTER_MODE.BoundsCheck(val);}
    void setCutoff(double val) {_cutoffHz = TPTSVFilterParameters::CUTOFF.BoundsCheck(val);}
    void setQ(double val) {_Q = TPTSVFilterParameters::Q.BoundsCheck(val);}
    void setGain(double val) {_gain = TPTSVFilterParameters::GAIN.BoundsCheck(val);}
    void setSampleRate(double val) {_sampleRate = val;}
    
private:
    double  _sampleRate,
            _cutoffHz,
            _Q,
            _gain,
            _s1,
            _s2;
    
    int _mode;
};



#endif  // TPTSVFILTER_H_INCLUDED
