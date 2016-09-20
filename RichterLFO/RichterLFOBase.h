/*
 *  File:       RichterLFOBase.h
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

#ifndef Richter_RichterLFOBase_h
#define Richter_RichterLFOBase_h

#define _USE_MATH_DEFINES

#include "math.h"
#include "ParameterData.h"

class RichterLFOBase {
protected:
    int manualPhase,
        wave,
        index,
        indexOffset,
        samplesProcessed;
    
    bool    bypassSwitch,
            tempoSyncSwitch,
            phaseSyncSwitch,
            needsPhaseCalc;
        
    float   tempoNumer,
            tempoDenom,
            tempoFreq,
            freq,
            depth,
            samplesPerTremoloCycle,
            gain,
            offset,
            currentScale,
            nextScale;
    
    float   *waveArrayPointer;
    
    float mSine[kWaveArraySize];
    float mSquare[kWaveArraySize];
    float mSaw[kWaveArraySize];
    
    template <typename T>
    T boundsCheck(T param, T min, T max) {
        if (param < min) param = min;
        if (param > max) param = max;
        
        return param;
    }
    
public:
    RichterLFOBase() :  manualPhase(0),
                        wave(WAVE_DEFAULT),
                        index(0),
                        indexOffset(0),
                        samplesProcessed(0),
                        bypassSwitch(LFOSWITCH_DEFAULT),
                        tempoSyncSwitch(TEMPOSYNC_DEFAULT),
                        phaseSyncSwitch(PHASESYNC_DEFAULT),
                        needsPhaseCalc(true),
                        tempoNumer(TEMPONUMER_DEFAULT),
                        tempoDenom(TEMPODENOM_DEFAULT),
                        tempoFreq(FREQ_DEFAULT),
                        freq(FREQ_DEFAULT),
                        depth(DEPTH_DEFAULT),
                        samplesPerTremoloCycle(1),
                        gain(1),
                        offset(0),
                        currentScale(0),
                        nextScale(0),
                        waveArrayPointer(&mSine[0]) {
    }
    
    virtual ~RichterLFOBase() {};
    
    // getter methods
    
    int getBypassSwitch() const { return bypassSwitch; }
    
    int getPhaseSyncSwitch() const { return phaseSyncSwitch; }
    
    int getTempoSyncSwitch() const { return tempoSyncSwitch; }
    
    float getWave() const { return wave; }
    
    float getDepth() const { return depth; }
    
    float getFreq() const { return freq; }
    
    int getManualPhase() const { return manualPhase; }
    
    float getTempoNumer() const { return tempoNumer; }
    
    float getTempoDenom() const { return tempoDenom; }
    
    float getWaveArraySize() const { return kWaveArraySize; }
    
    int getIndexOffset() { return indexOffset; }
        
    
    
    // set parameter methods, w/ integrated bounds checks
    
    void setBypassSwitch(bool val) { bypassSwitch = val; }
    
    void setPhaseSyncSwitch(bool val) { phaseSyncSwitch = val; }
    
    void setTempoSyncSwitch(bool val) { tempoSyncSwitch = val; }
    
    void setTempoNumer(int val) { tempoNumer = boundsCheck<int>(val, TEMPONUMER_MIN, TEMPONUMER_MAX); }
    
    void setTempoDenom (int val) { tempoDenom = boundsCheck<int>(val, TEMPODENOM_MIN, TEMPODENOM_MAX); }
    
    void setFreq(float val) { freq = boundsCheck(val, FREQ_MIN, FREQ_MAX); }
    
    void setDepth(float val) { depth = boundsCheck(val, DEPTH_MIN, DEPTH_MAX); }
    
    void setManualPhase(int val) { manualPhase = boundsCheck(val, PHASE_MIN, PHASE_MAX); }
    
    void setWave(float val) { wave = boundsCheck<int>(val, WAVE_MIN, WAVE_MAX); }
    
    void setWaveTablePointers() {
        if (wave == WAVE_SINE) { waveArrayPointer = &mSine[0]; }
        if (wave == WAVE_SQUARE) { waveArrayPointer = &mSquare[0]; }
        if (wave == WAVE_SAW) { waveArrayPointer = &mSaw[0]; }
    }
    
    void setIndexOffset(int val) { indexOffset = val; }
    
    
    /* reset
     *
     * Resets indexOffset and currentScale. Call before beginning a new buffer of
     * samples.
     */
    void reset() {
        needsPhaseCalc = true;
        indexOffset = 0;
        currentScale = 0;
        samplesProcessed = 0;
    }
    
    /* calcPhaseOffset
     *
     * Calculates the phase offset to be applied to the oscillator, including any
     * offset required by the phase sync and any offset applied by the user.
     *
     * args: timeInSeconds   Position of the host DAW's playhead at the start of
     *                       playback
     */
    void calcPhaseOffset(double timeInSeconds) {
        if (phaseSyncSwitch && needsPhaseCalc) {
            float waveLength {1 / freq};
            double waveTimePosition {0};
            
            if (waveLength < timeInSeconds) {
                waveTimePosition = fmod(timeInSeconds, waveLength);
            } else {
                waveTimePosition = timeInSeconds;
            }
            indexOffset = (waveTimePosition / waveLength) * kWaveArraySize + manualPhase;
        }
        
        if (!phaseSyncSwitch && needsPhaseCalc) {
            indexOffset = manualPhase;
        }
        needsPhaseCalc = false;
        
    }
    
    /* calcFreq
     *
     * Calculates the frequency of the oscillator. Will use either the frequency
     * or tempoNumer/tempoDenom depending on whether tempo sync is enabled.
     *
     * args: bpm   Current bpm of the host DAW
     */
    void calcFreq(double bpm) {
        // calculate the frequency based on whether tempo sync is active
        
        tempoFreq = (bpm / 60) * (tempoDenom / tempoNumer);
        
        if (tempoSyncSwitch) { freq = tempoFreq; }
        
        boundsCheck(freq, FREQ_MIN, FREQ_MAX);
        
    }
    
    /* calcSamplesPerTremoloCycle
     *
     * Calculates the number of samples which pass in the same time as one cycle
     * of the LFO. Dependant on the LFO frequency and the sample rate.
     *
     * args: sampleRate   Sample rate of the host DAW
     */
    void calcSamplesPerTremoloCycle(float sampleRate) {
        samplesPerTremoloCycle = sampleRate / freq;
    }
    
    /* calcIndexAndScaleInLoop
     *
     * Calculates the current index of the oscillator in its wavetable. Includes
     * protection against indexes out of range (caused by phase offset) and updates
     * currentScale. Call from within the processing loop. Increments the number of 
     * samples processed
     *
     */
    void calcIndexAndScaleInLoop() {
        // calculate the current index within the wave table
        
        index = static_cast<long>(samplesProcessed * currentScale) % kWaveArraySize;
        
        if ((nextScale != currentScale) && (index == 0)) {
            currentScale = nextScale;
            samplesProcessed = 0;
        }
        
        
        // Must provide two possibilities for each index lookup in order to protect the array from being overflowed by the indexOffset, the first if statement uses the standard index lookup while second if statement deals with the overflow possibility
        
        if ((index + indexOffset) < kWaveArraySize) {
            gain = waveArrayPointer[index + indexOffset];
        } else if ((index + indexOffset) >= kWaveArraySize) {
            gain = waveArrayPointer[(index + indexOffset) % kWaveArraySize];
        }
        
        samplesProcessed++;
    }
    
    /* calcNextScale
     *
     * Calculates the scale factor to be applied when calculating the index. 
     */
    void calcNextScale() {
        nextScale = kWaveArraySize / samplesPerTremoloCycle;
    }
    
    RichterLFOBase operator=(RichterLFOBase& other) = delete;
    RichterLFOBase(RichterLFOBase& other) = delete;
};

#endif
