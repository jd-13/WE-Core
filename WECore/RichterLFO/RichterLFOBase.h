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

#pragma once

#include "RichterParameters.h"
#include "General/CoreMath.h"

namespace WECore::Richter {

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
        RichterLFOBase() :  _manualPhase(Parameters::PHASE.defaultValue),
                            _wave(Parameters::WAVE.defaultValue),
                            _index(0),
                            _indexOffset(0),
                            _samplesProcessed(0),
                            _bypassSwitch(Parameters::LFOSWITCH_DEFAULT),
                            _tempoSyncSwitch(Parameters::TEMPOSYNC_DEFAULT),
                            _phaseSyncSwitch(Parameters::PHASESYNC_DEFAULT),
                            _needsPhaseCalc(true),
                            _tempoNumer(Parameters::TEMPONUMER.defaultValue),
                            _tempoDenom(Parameters::TEMPODENOM.defaultValue),
                            _tempoFreq(Parameters::FREQ.defaultValue),
                            _freq(Parameters::FREQ.defaultValue),
                            _depth(Parameters::DEPTH.defaultValue),
                            _samplesPerTremoloCycle(1),
                            _gain(1),
                            _offset(0),
                            _currentScale(0),
                            _nextScale(0),
                            _waveArrayPointer(&_sineTable[0]) {
        }

        virtual ~RichterLFOBase() {}

        friend class RichterLFOPair;

        /** @name Getter Methods */
        /** @{ */

        bool getBypassSwitch() const { return _bypassSwitch; }

        bool getPhaseSyncSwitch() const { return _phaseSyncSwitch; }

        bool getTempoSyncSwitch() const { return _tempoSyncSwitch; }

        int getWave() const { return _wave; }

        double getDepth() const { return _depth; }

        double getFreq() const { return _freq; }

        int getManualPhase() const { return _manualPhase; }

        double getTempoNumer() const { return _tempoNumer; }

        double getTempoDenom() const { return _tempoDenom; }

        float getWaveArraySize() const { return Parameters::kWaveArraySize; }

        int getIndexOffset() { return _indexOffset; }

        /** @} */

        /** @name Setter Methods */
        /** @{ */

        void setBypassSwitch(bool val) { _bypassSwitch = val; }

        void setPhaseSyncSwitch(bool val) { _phaseSyncSwitch = val; }

        void setTempoSyncSwitch(bool val) { _tempoSyncSwitch = val; }

        void setTempoNumer(int val) { _tempoNumer = Parameters::TEMPONUMER.BoundsCheck(val); }

        void setTempoDenom (int val) { _tempoDenom = Parameters::TEMPODENOM.BoundsCheck(val); }

        void setFreq(double val) { _freq = Parameters::FREQ.BoundsCheck(val); }

        void setDepth(double val) { _depth = Parameters::DEPTH.BoundsCheck(val); }

        void setManualPhase(int val) { _manualPhase = static_cast<int>(Parameters::PHASE.BoundsCheck(val)); }

        void setWave(int val) { _wave = Parameters::WAVE.BoundsCheck(val); }

        inline void setWaveTablePointers();

        void setIndexOffset(int val) { _indexOffset = val; }

        /** @} */

        /**
         * Prepares for processing the next buffer of samples. For example if using JUCE, you
         * would call this in your processBlock() method before doing any processing.
         *
         * @param   bpm             Current bpm of the host
         * @param   timeInSeconds   Position of the host DAW's playhead at the start of
         *                          playback.
         * @param   sampleRate      Current sample rate of the host
         */
        inline void prepareForNextBuffer(double bpm, double timeInSeconds, double sampleRate);

        /**
         * Must be called before beginning a new buffer of samples.
         * Resets internal counters including indexOffset and currentScale.
         */
        inline void reset();

        RichterLFOBase operator=(RichterLFOBase& other) = delete;
        RichterLFOBase(RichterLFOBase& other) = delete;

    protected:
        int     _manualPhase,
                _wave,
                _index,
                _indexOffset;

        long    _samplesProcessed;

        bool    _bypassSwitch,
                _tempoSyncSwitch,
                _phaseSyncSwitch,
                _needsPhaseCalc;

        double  _tempoNumer,
                _tempoDenom,
                _tempoFreq,
                _freq,
                _depth,
                _samplesPerTremoloCycle,
                _gain,
                _offset,
                _currentScale,
                _nextScale;

        double* _waveArrayPointer;

        double _sineTable[Parameters::kWaveArraySize];
        double _squareTable[Parameters::kWaveArraySize];
        double _sawTable[Parameters::kWaveArraySize];

        /**
         * Calculates the phase offset to be applied to the oscillator, including any
         * offset required by the phase sync and any offset applied by the user.
         *
         * @param   timeInSeconds   Position of the host DAW's playhead at the start of
         *                          playback.
         */
        inline void _calcPhaseOffset(double timeInSeconds);

        /**
         * Calculates the frequency of the oscillator. Will use either the frequency
         * or tempoNumer/tempoDenom depending on whether tempo sync is enabled.
         *
         * @param   bpm   Current bpm of the host DAW
         */
        inline void _calcFreq(double bpm);

        /**
         * Calculates the number of samples which pass in the same time as one cycle
         * of the LFO. Dependant on the LFO frequency and the sample rate.
         *
         * @param   sampleRate   Sample rate of the host DAW
         */
        void _calcSamplesPerTremoloCycle(double sampleRate) {
            _samplesPerTremoloCycle = sampleRate / _freq;
        }

        /**
         * Calculates the scale factor to be applied when calculating the index.
         */
        void _calcNextScale() {
            _nextScale = Parameters::kWaveArraySize / _samplesPerTremoloCycle;
        }

        /**
         * Calculates the current index of the oscillator in its wavetable. Includes
         * protection against indexes out of range (caused by phase offset) and updates
         * currentScale. Call from within the processing loop. Increments the number of
         * samples processed
         *
         */
        inline void calcIndexAndScaleInLoop();
    };

    void RichterLFOBase::setWaveTablePointers() {
        if (_wave == Parameters::WAVE.SINE) { _waveArrayPointer = &_sineTable[0]; }
        if (_wave == Parameters::WAVE.SQUARE) { _waveArrayPointer = &_squareTable[0]; }
        if (_wave == Parameters::WAVE.SAW) { _waveArrayPointer = &_sawTable[0]; }
    }



    void RichterLFOBase::prepareForNextBuffer(double bpm,
                                              double timeInSeconds,
                                              double sampleRate) {
        setWaveTablePointers();
        _calcFreq(bpm);
        _calcPhaseOffset(timeInSeconds);
        _calcSamplesPerTremoloCycle(sampleRate);
        _calcNextScale();
    }


    void RichterLFOBase::reset() {
        _needsPhaseCalc = true;
        _indexOffset = 0;
        _currentScale = 0;
        _samplesProcessed = 0;
    }


    void RichterLFOBase::_calcPhaseOffset(double timeInSeconds) {
        if (_phaseSyncSwitch && _needsPhaseCalc) {
            static double waveLength {1 / _freq};
            static double waveTimePosition {0};

            if (waveLength < timeInSeconds) {
                waveTimePosition = fmod(timeInSeconds, waveLength);
            } else {
                waveTimePosition = timeInSeconds;
            }
            _indexOffset = static_cast<int>(waveTimePosition / waveLength) * Parameters::kWaveArraySize + _manualPhase;
        }

        if (!_phaseSyncSwitch && _needsPhaseCalc) {
            _indexOffset = _manualPhase;
        }
        _needsPhaseCalc = false;

    }


    void RichterLFOBase::_calcFreq(double bpm) {
        // calculate the frequency based on whether tempo sync is active

        _tempoFreq = (bpm / 60) * (_tempoDenom / _tempoNumer);

        if (_tempoSyncSwitch) { _freq = _tempoFreq; }

        _freq = Parameters::FREQ.BoundsCheck(_freq);

    }

    void RichterLFOBase::calcIndexAndScaleInLoop() {
        // calculate the current index within the wave table
        _index = static_cast<int>(static_cast<long>(_samplesProcessed * static_cast<long double>(_currentScale)) % Parameters::kWaveArraySize);

        if ((!CoreMath::compareFloatsEqual(_nextScale, _currentScale)) && (_index == 0)) {
            _currentScale = _nextScale;
            _samplesProcessed = 0;
        }


        // Must provide two possibilities for each index lookup in order to protect the array from being overflowed by the indexOffset, the first if statement uses the standard index lookup while second if statement deals with the overflow possibility

        if ((_index + _indexOffset) < Parameters::kWaveArraySize) {
            _gain = _waveArrayPointer[_index + _indexOffset];
        } else if ((_index + _indexOffset) >= Parameters::kWaveArraySize) {
            _gain = _waveArrayPointer[(_index + _indexOffset) % Parameters::kWaveArraySize];
        }

        _samplesProcessed++;
    }
}
