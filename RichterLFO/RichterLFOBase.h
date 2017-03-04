/*
 *  File:       RichterLFOBase.h
 *
 *  Version:    2.0.0
 *
 *  Created:    18/01/2015
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

#ifndef RichterLFOBase_h
#define RichterLFOBase_h

#define _USE_MATH_DEFINES

#include "math.h"
#include "RichterParameters.h"

class RichterLFOPair;

/**
 * A bass class for providing most of the functionality needed for an LFO.
 * This cannot be used used directly "as-is" in your project. For an LFO 
 * which can be dropped straight into your project, see RichterLFO or RichterMOD,
 * which complete the implementation of this class.
 *
 * Supports multiple wave shapes, tempo sync, and phase sync for consistent
 * playback no matter where the host playhead started playback from.
 */
class RichterLFOBase {
public:
    
    /**
     * Initialises parameters to default values.
     */
    RichterLFOBase() :  manualPhase(0),
                        wave(WAVE.defaultValue),
                        index(0),
                        indexOffset(0),
                        samplesProcessed(0),
                        bypassSwitch(LFOSWITCH_DEFAULT),
                        tempoSyncSwitch(TEMPOSYNC_DEFAULT),
                        phaseSyncSwitch(PHASESYNC_DEFAULT),
                        needsPhaseCalc(true),
                        tempoNumer(TEMPONUMER.defaultValue),
                        tempoDenom(TEMPODENOM.defaultValue),
                        tempoFreq(FREQ.defaultValue),
                        freq(FREQ.defaultValue),
                        depth(DEPTH.defaultValue),
                        samplesPerTremoloCycle(1),
                        gain(1),
                        offset(0),
                        currentScale(0),
                        nextScale(0),
                        waveArrayPointer(&mSine[0]) {
    }
    
    virtual ~RichterLFOBase() {}
    
    friend class RichterLFOPair;
    
    // getter methods
    
    bool getBypassSwitch() const { return bypassSwitch; }
    
    bool getPhaseSyncSwitch() const { return phaseSyncSwitch; }
    
    bool getTempoSyncSwitch() const { return tempoSyncSwitch; }
    
    int getWave() const { return wave; }
    
    double getDepth() const { return depth; }
    
    double getFreq() const { return freq; }
    
    int getManualPhase() const { return manualPhase; }
    
    double getTempoNumer() const { return tempoNumer; }
    
    double getTempoDenom() const { return tempoDenom; }
    
    float getWaveArraySize() const { return kWaveArraySize; }
    
    int getIndexOffset() { return indexOffset; }
        
    
    
    // set parameter methods, w/ integrated bounds checks
    
    void setBypassSwitch(bool val) { bypassSwitch = val; }
    
    void setPhaseSyncSwitch(bool val) { phaseSyncSwitch = val; }
    
    void setTempoSyncSwitch(bool val) { tempoSyncSwitch = val; }
    
    void setTempoNumer(int val) { tempoNumer = TEMPONUMER.BoundsCheck(val); }
    
    void setTempoDenom (int val) { tempoDenom = TEMPODENOM.BoundsCheck(val); }
    
    void setFreq(double val) { freq = FREQ.BoundsCheck(val); }
    
    void setDepth(double val) { depth = DEPTH.BoundsCheck(val); }
    
    void setManualPhase(int val) { manualPhase = static_cast<int>(PHASE.BoundsCheck(val)); }
    
    void setWave(int val) { wave = WAVE.BoundsCheck(val); }
    
    void setWaveTablePointers() {
        if (wave == WAVE.SINE) { waveArrayPointer = &mSine[0]; }
        if (wave == WAVE.SQUARE) { waveArrayPointer = &mSquare[0]; }
        if (wave == WAVE.SAW) { waveArrayPointer = &mSaw[0]; }
    }
    
    void setIndexOffset(int val) { indexOffset = val; }
    
    /**
     * Prepares for processing the next buffer of samples. For example if using JUCE, you 
     * would call this in your processBlock() method before doing any processing.
     *
     * @param   bpm             Current bpm of the host
     * @param   timeInSeconds   Position of the host DAW's playhead at the start of
     *                          playback.
     * @param   sampleRate      Current sample rate of the host
     */
    void prepareForNextBuffer(double bpm,
                              double timeInSeconds,
                              double sampleRate) {
        setWaveTablePointers();
        calcFreq(bpm);
        calcPhaseOffset(timeInSeconds);
        calcSamplesPerTremoloCycle(sampleRate);
        calcNextScale();
    }
    
    /**
     * Must be called before beginning a new buffer of samples.
     * Resets internal counters including indexOffset and currentScale.
     */
    void reset() {
        needsPhaseCalc = true;
        indexOffset = 0;
        currentScale = 0;
        samplesProcessed = 0;
    }
    
    RichterLFOBase operator=(RichterLFOBase& other) = delete;
    RichterLFOBase(RichterLFOBase& other) = delete;
    
protected:
    int     manualPhase,
            wave,
            index,
            indexOffset;
    
    long    samplesProcessed;
    
    bool    bypassSwitch,
            tempoSyncSwitch,
            phaseSyncSwitch,
            needsPhaseCalc;
    
    double  tempoNumer,
            tempoDenom,
            tempoFreq,
            freq,
            depth,
            samplesPerTremoloCycle,
            gain,
            offset,
            currentScale,
            nextScale;
    
    double *waveArrayPointer;
    
    double mSine[kWaveArraySize];
    double mSquare[kWaveArraySize];
    double mSaw[kWaveArraySize];
    
    /**
     * Calculates the phase offset to be applied to the oscillator, including any
     * offset required by the phase sync and any offset applied by the user.
     *
     * @param   timeInSeconds   Position of the host DAW's playhead at the start of
     *                          playback.
     */
    void calcPhaseOffset(double timeInSeconds) {
        if (phaseSyncSwitch && needsPhaseCalc) {
            static double waveLength {1 / freq};
            static double waveTimePosition {0};
            
            if (waveLength < timeInSeconds) {
                waveTimePosition = fmod(timeInSeconds, waveLength);
            } else {
                waveTimePosition = timeInSeconds;
            }
            indexOffset = static_cast<int>(waveTimePosition / waveLength) * kWaveArraySize + manualPhase;
        }
        
        if (!phaseSyncSwitch && needsPhaseCalc) {
            indexOffset = manualPhase;
        }
        needsPhaseCalc = false;
        
    }
    
    /**
     * Calculates the frequency of the oscillator. Will use either the frequency
     * or tempoNumer/tempoDenom depending on whether tempo sync is enabled.
     *
     * @param   bpm   Current bpm of the host DAW
     */
    void calcFreq(double bpm) {
        // calculate the frequency based on whether tempo sync is active
        
        tempoFreq = (bpm / 60) * (tempoDenom / tempoNumer);
        
        if (tempoSyncSwitch) { freq = tempoFreq; }
        
        freq = FREQ.BoundsCheck(freq);
        
    }
    
    /**
     * Calculates the number of samples which pass in the same time as one cycle
     * of the LFO. Dependant on the LFO frequency and the sample rate.
     *
     * @param   sampleRate   Sample rate of the host DAW
     */
    void calcSamplesPerTremoloCycle(double sampleRate) {
        samplesPerTremoloCycle = sampleRate / freq;
    }
    
    /**
     * Calculates the scale factor to be applied when calculating the index.
     */
    void calcNextScale() {
        nextScale = kWaveArraySize / samplesPerTremoloCycle;
    }
    
    /**
     * Calculates the current index of the oscillator in its wavetable. Includes
     * protection against indexes out of range (caused by phase offset) and updates
     * currentScale. Call from within the processing loop. Increments the number of
     * samples processed
     *
     */
    void calcIndexAndScaleInLoop() {
        // calculate the current index within the wave table
        index = static_cast<int>(static_cast<long>(samplesProcessed * static_cast<long double>(currentScale)) % kWaveArraySize);
        
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
};

#endif /* RichterLFOBase_h */
