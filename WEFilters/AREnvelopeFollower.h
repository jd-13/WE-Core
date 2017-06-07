/*
 *  File:       AREnveloperFollower.h
 *
 *  Version:    1.0.0
 *
 *  Created:    27/05/2017
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

#ifndef ARENVELOPEFOLLOWER_H_INCLUDED
#define ARENVELOPEFOLLOWER_H_INCLUDED

#include "General/CoreMath.h"
#include "WEFilters/AREnvelopeFollowerParameters.h"

/**
 * A basic envelope follower with controls for attack and release times.
 */
class AREnvelopeFollower {
public:
    AREnvelopeFollower() : _envVal(0),
                           _attackTimeMs(AREnvelopeFollowerParameters::ATTACK_MS.defaultValue),
                           _releaseTimeMs(AREnvelopeFollowerParameters::RELEASE_MS.defaultValue) {
        // call this here rather than setting it in initialiser list so that the coefficients get
        // setup
        setSampleRate(44100);
    }
    
    /** @name Setter Methods */
    /** @{ */
    
    void setSampleRate(double sampleRate) {
        _sampleRate = sampleRate;
        _attackCoef = _calcCoef(_attackTimeMs);
        _releaseCoef = _calcCoef(_releaseTimeMs);
    }
    
    void setAttackTimeMs(double time) {
        _attackTimeMs = time;
        _attackCoef = _calcCoef(_attackTimeMs);
        
    }
    void setReleaseTimeMs(double time) {
        _releaseTimeMs = time;
        _releaseCoef = _calcCoef(_releaseTimeMs);
    }
    
    /** @} */
    
    /**
     * Updates the envelope with the current sample and returns the updated envelope value. Must be
     * called for every sample.
     *
     * @param[in]   inSample    Sample used to update the envelope state
     *
     * @return  The updated envelope value
     */
    double clockUpdateEnvelope(double inSample) {
        double tmp = std::abs(inSample);
        if(tmp > _envVal)
            _envVal = _attackCoef * (_envVal - tmp) + tmp;
        else
            _envVal = _releaseCoef * (_envVal - tmp) + tmp;
        
        return _envVal;
    }
    
    /**
     * Returns the latest envelope value without modifying it.
     */
    double getEnvelope() { return _envVal; }
    
    
private:
    double _envVal;
    
    double _attackTimeMs;
    double _releaseTimeMs;
    
    double _attackCoef;
    double _releaseCoef;
    
    double _sampleRate;
    
    double _calcCoef(double timeMs) {
        return exp(log(0.01)/( timeMs * _sampleRate * 0.001));
    }
};


#endif  // ARENVELOPEFOLLOWER_H_INCLUDED
