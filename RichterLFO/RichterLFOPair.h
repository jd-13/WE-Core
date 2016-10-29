/*
 *  File:       RichterLFOPair.h
 *
 *  Version:    2.0.0
 *
 *  Created:    18/05/2015
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

#ifndef RICHTERLFOPAIR_H_INCLUDED
#define RICHTERLFOPAIR_H_INCLUDED

#include "RichterLFO.h"
#include "RichterMOD.h"

/**
 *
 * A convenience class that allows a simple implementation of an LFO that has
 * been paired with a MOD oscillator to modulate its depth and frequency. If you use
 * this class, you will never need to interact directly with either of the contained
 * LFOs for anything other than getting or setting parameter values.
 *
 * This class has been created as the LFO relies on the MOD being ready before
 * it can perform certain operations, which means there are method calls to 
 * each oscillator which must be interleaved carefully.
 */
 
class RichterLFOPair {
public:
    RichterLFOPair() : _LFO(), _MOD() {}
    
    virtual ~RichterLFOPair() {}
    
    /**
     * Call each oscillator's reset method.
     */
    void reset() {
        _LFO.reset();
        _MOD.reset();
    }
    
    /**
     * Prepares for processing the next buffer of samples. For example if using JUCE, you
     * would call this in your processBlock() method before doing any processing.
     *
     * This calls various protected methods of each of the oscillators in a specific order
     * to ensure calculations are done correctly.
     *
     * @param   bpm             Current bpm of the host.
     * @param   timeInSeconds   Position of the host DAW's playhead at the start of
     *                          playback.
     * @param   sampleRate      Current sample rate of the host
     */
    void prepareForNextBuffer(double bpm,
                              double timeInSeconds,
                              double sampleRate) {
        _LFO.setWaveTablePointers();
        _MOD.setWaveTablePointers();
        
        _MOD.calcFreq(bpm);
        _MOD.calcPhaseOffset(timeInSeconds);
        
        _LFO.calcFreq(bpm);
        _LFO.calcPhaseOffset(timeInSeconds);
        
        _LFO.calcSamplesPerTremoloCycle(sampleRate);
        _MOD.calcSamplesPerTremoloCycle(sampleRate);
        
        _LFO.calcNextScale();
        _MOD.calcNextScale();
    }
    
    /**
     * Use this in your processing loop. Returns a gain value which is intended to be
     * multiplied with a single sample to apply the tremolo effect.
     *
     * Note: Calling this method will advance the oscillators internal counters by one
     *       sample. Calling this method will return a different value each time.
     *
     * @return  The value of the RichterLFO's output at this moment, a value between 0 and 1.
     */
    double calcGainInLoop() {
        _LFO.calcIndexAndScaleInLoop();
        _MOD.calcIndexAndScaleInLoop();
        
        return _LFO.calcGain(_MOD.getBypassSwitch(), _MOD.calcGain());
    }
    
    
    RichterLFO _LFO;
    RichterMOD _MOD;
    
    RichterLFOPair operator= (RichterLFOPair& other) = delete;
    RichterLFOPair(RichterLFOPair& other) = delete;
    
};



#endif  // RICHTERLFOPAIR_H_INCLUDED
