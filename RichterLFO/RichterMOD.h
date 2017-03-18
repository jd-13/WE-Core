/*
 *  File:       RichterMOD.h
 *
 *  Version:    2.0.0
 *
 *  Created:    05/02/2015
 *
 *	This file is part of the WECore.
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

#ifndef Richter_RichterMOD_h
#define Richter_RichterMOD_h

#include "RichterLFOBase.h"

class RichterLFOPair;

/**
 * Provides and LFO with: depth, rate, tempo sync, phase, wave shape,
 * and phase sync controls. This LFO oscillates between -1 and 1, and so
 * is useful as a modulation source for parameters.
 *
 * To use, you simply need to call reset, prepareForNextBuffer, and calcGainInLoop
 * as necessary (see their descriptions for details), and use the provided getter
 * and setter methods to manipulate parameters.
 *
 * Completes the implementation of RichterLFO.
 */
class RichterMOD : public RichterLFOBase {
    
public:
    /**
     * Generates the wave tables on initialsation, while running gain values
     * are simply looked up from these wave tables.
     */
    RichterMOD() : RichterLFOBase() {
        
        // mod wavetables below (no correction, move below 0)
        
        for (int i = 0; i < kWaveArraySize; ++i) {
            
            // sine wavetable
            double radians {i * 2.0 * CoreMath::DOUBLE_PI / kWaveArraySize};
            mSine[i] = (sin (radians)) * 0.5;
            
            // square wavetable
            double squareRadians {radians + 0.32};
            mSquare[i] =
            (
             sin (radians) +
             0.3 * sin (3 * squareRadians) +
             0.15 * sin (5 * squareRadians) +
             0.075 * sin (7 * squareRadians) +
             0.0375 * sin (9 * squareRadians) +
             0.01875 * sin (11 * squareRadians) +
             0.009375 * sin (13 * squareRadians)
             );
            
            // saw wavetable
            double sawRadians {radians + CoreMath::DOUBLE_PI};
            mSaw[i] =
            (1/CoreMath::DOUBLE_PI) *
            (
             sin (sawRadians) -
             0.5 * sin (2 * sawRadians) +
             (1.0/3.0) * sin (3 * sawRadians) -
             0.25 * sin (4 * sawRadians) +
             0.2 * sin (5 * sawRadians) -
             (1.0/6.0) * sin (6 * sawRadians) +
             (1.0/7.0) * sin (7 * sawRadians) -
             (1.0/8.0) * sin (8 * sawRadians) +
             (1.0/9.0) * sin (9 * sawRadians) -
             0.1 * sin (10 * sawRadians) +
             (1.0/11.0) * sin (11 * sawRadians) -
             (1.0/12.0) * sin (12 * sawRadians) +
             (1.0/13.0) * sin (13 * sawRadians) -
             (1.0/14.0) * sin (14 * sawRadians)
             );
        }
    }
    
    virtual ~RichterMOD() {}
    
    friend class RichterLFOPair;
    
    /**
     * Use this in your processing loop.
     *
     * Note: Calling this method will advance the oscillators internal counters by one
     *       sample. Calling this method will return a different value each time.
     *
     * @return  The value of the LFO's output at this moment, a value between -1 and 1.
     */
    double calcGainInLoop() {
        calcIndexAndScaleInLoop();
        return calcGain();
    }
    
    RichterMOD operator=(RichterMOD& other) = delete;
    RichterMOD(RichterMOD& other) = delete;
    
private:
    /**
     * Calculates the gain value to be applied to a signal (in this case a
     * parameter of another oscillator) which the oscillator is operating on.
     * Outputs a value between -0.5 and 0.5. Always outputs 0 if bypassed.
     */
    double calcGain() {
        if (bypassSwitch) {
            return (gain * depth);
        } else {
            return 0;
        }
    }
    
};


#endif
