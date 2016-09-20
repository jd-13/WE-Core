/*
 *  File:       RichterLFO.h
 *
 *  Version:    2.0.0
 *
 *  Created:    18/01/2015
 *
 *	This file is part of Richter.
 *
 *  Richter is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Richter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Richter.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Richter_RichterLFO_h
#define Richter_RichterLFO_h

#include "RichterLFOBase.h"

class RichterLFO : public RichterLFOBase {
    
public:
    
    RichterLFO() :  RichterLFOBase(),
    rawFreq(FREQ_DEFAULT),
    freqMod(FREQMOD_DEFAULT),
    rawDepth(DEPTH_DEFAULT),
    depthMod(DEPTHMOD_DEFAULT) {
        
        
        // initialise wavetable array values
        
        for (int i = 0; i < kWaveArraySize; ++i) {
            
            // sine wavetable
            double radians {i * 2.0 * M_PI / kWaveArraySize};
            mSine[i] = (sin (radians) + 1.0) * 0.5;
            
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
             0.009375 * sin (13 * squareRadians) +
             0.8
             ) * 0.63;
            
            // saw wavetable
            double sawRadians {radians + M_PI};
            mSaw[i] =
            (1/M_PI) *
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
             + 1.65
             );
        }
    }
    
    float getRawDepth() { return rawDepth; }
    
    float getDepthMod() { return depthMod; }
    
    float getRawFreq() { return rawFreq; }
    
    float getFreqMod() { return freqMod; }
    
    
    
    // set parameter methods, w/ integrated bounds checks
    
    void setRawFreq(float val) {
        rawFreq = val;
        rawFreq = boundsCheck(rawFreq, FREQ_MIN, FREQ_MAX);
    }
    
    void setFreqMod(float val) {
        freqMod = val;
        freqMod = boundsCheck(freqMod, FREQMOD_MIN, FREQMOD_MAX);
    }
    
    void setRawDepth(float val) {
        rawDepth = val;
        rawDepth = boundsCheck(rawDepth, DEPTH_MIN, DEPTH_MAX);
    }
    
    void setDepthMod(float val) {
        depthMod = val;
        depthMod = boundsCheck(depthMod, DEPTHMOD_MIN, DEPTHMOD_MAX);
    }
    
    /* calcGain
     *
     * Calculates the gain value to be applied to a signal which the oscillator
     * is operating on. Outputs a value between 0 and 1. Always outputs 1 if bypassed.
     *
     * args: modBypassSwitch   The state of the modulation oscillator. Determines
     *                         whether modGain is applied to the calculation
     *       modGain           The gain output from the modulation oscillator
     */
    float calcGain(int modBypassSwitch, float modGain) {
        calcFreqInLoop(modBypassSwitch, modGain);
        calcDepthInLoop(modBypassSwitch, modGain);
        
        if (bypassSwitch) {
            return ((gain * depth - depth + 1));
        } else {
            return 1;
        }
    }
    
    RichterLFO operator= (RichterLFO& other) = delete;
    RichterLFO(RichterLFO&) = delete;
    
private:
    
    float   rawFreq,
            freqMod,
            rawDepth,
            depthMod;
    
    /* calcFreqInLoop
     *
     * Applies frequency modulation to the oscillator. Performed in the processing
     * loop so that the frequency can be updated before processing each sample.
     *
     * args: modBypassSwitch   The state of the modulation oscillator. Determines
     *                         whether modGain is applied to the calculation
     *       modGain           The gain output from the modulation oscillator
     */
    void calcFreqInLoop(int modBypassSwitch, float modGain) {
        // calculate the frequency based on whether tempo sync or frequency modulation is active
        
        if (!tempoSyncSwitch) {
            if (modBypassSwitch) {
                freq = rawFreq + (freqMod * (FREQ_MAX / 2) * modGain);
            } else {
                freq = rawFreq;
            }
        }
        
        // Bounds check frequency after the modulation is applied to it
        freq = boundsCheck(freq, FREQ_MIN, FREQ_MAX);
        
    }
    
    /* calcDepthInLoop
     *
     * Applies depth modulation to the oscillator. Performed in the processing
     * loop so that the frequency can be updated before processing each sample.
     *
     * args: modBypassSwitch   The state of the modulation oscillator. Determines
     *                         whether modGain is applied to the calculation
     *       modGain           The gain output from the modulation oscillator
     */
    void calcDepthInLoop(int modBypassSwitch, float modGain) {
        // Check whether MOD oscs are activated and apply depth parameter modulation accordingly
        
        if (modBypassSwitch) {
            depth = rawDepth + (depthMod * DEPTH_MAX * modGain);
        } else {
            depth = rawDepth;
        }
        
        depth = boundsCheck(depth, DEPTH_MIN, DEPTH_MAX);
        
    }
};

#endif
