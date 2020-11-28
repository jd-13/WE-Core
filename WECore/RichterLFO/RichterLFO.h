/*
 *  File:       RichterLFO.h
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
#include "RichterWavetables.h"
#include "WEFilters/ModulationSource.h"

namespace WECore::Richter {

    class RichterLFOPair;

    /**
     * Provides and LFO with: depth, rate, tempo sync, phase, wave shape, and phase sync
     * controls, plus additional functionality to allow the depth and rate controls to be
     * modulated by an external source, with internal controls of the for the depth of this
     * modulation.
     *
     * This LFO oscillates between -1 and 1.
     *
     * To use, you simply need to call reset, prepareForNextBuffer, and getNextOutput
     * as necessary (see their descriptions for details), and use the provided getter
     * and setter methods to manipulate parameters.
     */
    class RichterLFO : public ModulationSource<double> {

    public:

        /**
         * Generates the wave tables on initialsation, while running gain values
         * are simply looked up from these wave tables.
         *
         * Also initialises parameters (that are not part of RichterLFOBase) to default values.
         */
        inline RichterLFO();

        virtual ~RichterLFO() override = default;

        friend class RichterLFOPair;

        /** @name Getter Methods */
        /** @{ */
        bool getBypassSwitch() const { return _bypassSwitch; }
        bool getPhaseSyncSwitch() const { return _phaseSyncSwitch; }
        bool getTempoSyncSwitch() const { return _tempoSyncSwitch; }
        bool getInvertSwitch() const { return _invertSwitch; }
        int getWave() const { return _wave; }
        double getTempoNumer() const { return _tempoNumer; }
        double getTempoDenom() const { return _tempoDenom; }
        double getFreq() { return _rawFreq; }
        double getFreqMod() { return _freqMod; }
        double getDepth() { return _rawDepth; }
        double getDepthMod() { return _depthMod; }
        int getManualPhase() const { return _manualPhase; }
        int getIndexOffset() { return _indexOffset; }
        /** @} */

        /** @name Setter Methods */
        /** @{ */
        void setBypassSwitch(bool val) { _bypassSwitch = val; }
        void setPhaseSyncSwitch(bool val) { _phaseSyncSwitch = val; }
        void setTempoSyncSwitch(bool val) { _tempoSyncSwitch = val; }
        void setInvertSwitch(bool val) { _invertSwitch = val; }
        inline void setWave(int val);
        void setTempoNumer(int val) { _tempoNumer = Parameters::TEMPONUMER.BoundsCheck(val); }
        void setTempoDenom (int val) { _tempoDenom = Parameters::TEMPODENOM.BoundsCheck(val); }
        void setFreq(double val) { _rawFreq = Parameters::FREQ.BoundsCheck(val); }
        void setFreqMod(double val) { _freqMod = Parameters::FREQMOD.BoundsCheck(val); }
        void setDepth(double val) { _rawDepth = Parameters::DEPTH.BoundsCheck(val); }
        void setDepthMod(double val) { _depthMod = Parameters::DEPTHMOD.BoundsCheck(val); }
        void setManualPhase(int val) { _manualPhase = static_cast<int>(Parameters::PHASE.BoundsCheck(val)); }
        void setIndexOffset(int val) { _indexOffset = val; }

        void setModulationSource(std::shared_ptr<ModulationSource> val) { _modulationSource = val; }
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

        RichterLFO operator= (RichterLFO& other) = delete;
        RichterLFO(RichterLFO&) = delete;

    private:
        int     _manualPhase,
                _wave,
                _index,
                _indexOffset;

        long    _samplesProcessed;

        bool    _bypassSwitch,
                _tempoSyncSwitch,
                _phaseSyncSwitch,
                _invertSwitch,
                _needsPhaseCalc;

        double  _tempoNumer,
                _tempoDenom,
                _freq,
                _rawFreq,
                _freqMod,
                _rawDepth,
                _depthMod,
                _samplesPerTremoloCycle,
                _gain,
                _currentScale,
                _nextScale;

        const double* _waveArrayPointer;


        // It may make more sense for this to be a weak_ptr, but weak_ptr.lock() seems to come with
        // a performance penalty
        std::shared_ptr<ModulationSource> _modulationSource;

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
            _nextScale = Wavetables::SIZE / _samplesPerTremoloCycle;
        }

        /**
         * Calculates the current index of the oscillator in its wavetable. Includes
         * protection against indexes out of range (caused by phase offset) and updates
         * currentScale. Call from within the processing loop. Increments the number of
         * samples processed
         *
         */
        inline void calcIndexAndScaleInLoop();

        /**
         * Returns the next output of the LFO.
         *
         * Note: Calling this method will advance the oscillators internal counters by one
         *       sample. Calling this method will return a different value each time.
         *
         * @return  The value of the LFO's output at this moment, a value between -1 and 1.
         */
        inline double _getNextOutputImpl(double inSample) override;

        /**
         * Applies frequency modulation to the oscillator. Performed in the processing
         * loop so that the frequency can be updated before processing each sample.
         *
         * @param   modGain             The gain output from the modulation oscillator.
         */
        inline void _calcFreqInLoop(double modGain);

        /**
         * Resets internal counters including indexOffset and currentScale.
         */
        virtual inline void _resetImpl() override;

    };

    RichterLFO::RichterLFO() : _manualPhase(static_cast<int>(Parameters::PHASE.defaultValue)),
                               _wave(Parameters::WAVE.defaultValue),
                               _index(0),
                               _indexOffset(0),
                               _samplesProcessed(0),
                               _bypassSwitch(Parameters::LFOSWITCH_DEFAULT),
                               _tempoSyncSwitch(Parameters::TEMPOSYNC_DEFAULT),
                               _phaseSyncSwitch(Parameters::PHASESYNC_DEFAULT),
                               _invertSwitch(Parameters::INVERT_DEFAULT),
                               _needsPhaseCalc(true),
                               _tempoNumer(Parameters::TEMPONUMER.defaultValue),
                               _tempoDenom(Parameters::TEMPODENOM.defaultValue),
                               _freq(Parameters::FREQ.defaultValue),
                               _rawFreq(Parameters::FREQ.defaultValue),
                               _freqMod(Parameters::FREQMOD.defaultValue),
                               _rawDepth(Parameters::DEPTH.defaultValue),
                               _depthMod(Parameters::DEPTHMOD.defaultValue),
                               _samplesPerTremoloCycle(1),
                               _gain(1),
                               _currentScale(0),
                               _nextScale(0),
                               _waveArrayPointer(Wavetables::getInstance()->getSine()),
                               _modulationSource(nullptr) {
    }

    void RichterLFO::setWave(int val) {
        _wave = Parameters::WAVE.BoundsCheck(val);

        if (_wave == Parameters::WAVE.SINE) {
            _waveArrayPointer = Wavetables::getInstance()->getSine();
        } else if (_wave == Parameters::WAVE.SQUARE) {
            _waveArrayPointer = Wavetables::getInstance()->getSquare();
        } else if (_wave == Parameters::WAVE.SAW) {
            _waveArrayPointer = Wavetables::getInstance()->getSaw();
        } else if (_wave == Parameters::WAVE.SIDECHAIN) {
            _waveArrayPointer = Wavetables::getInstance()->getSidechain();
        }
    }

    void RichterLFO::prepareForNextBuffer(double bpm,
                                              double timeInSeconds,
                                              double sampleRate) {
        _calcFreq(bpm);
        _calcPhaseOffset(timeInSeconds);
        _calcSamplesPerTremoloCycle(sampleRate);
        _calcNextScale();
    }

    void RichterLFO::_resetImpl() {
        _needsPhaseCalc = true;
        _indexOffset = 0;
        _currentScale = 0;
        _samplesProcessed = 0;
    }

    void RichterLFO::_calcPhaseOffset(double timeInSeconds) {
        if (_phaseSyncSwitch && _needsPhaseCalc) {
            double waveLength {1 / _freq};
            double waveTimePosition {0};

            if (waveLength < timeInSeconds) {
                waveTimePosition = fmod(timeInSeconds, waveLength);
            } else {
                waveTimePosition = timeInSeconds;
            }
            _indexOffset = static_cast<int>(waveTimePosition / waveLength) * Wavetables::SIZE + _manualPhase;
        }

        if (!_phaseSyncSwitch && _needsPhaseCalc) {
            _indexOffset = _manualPhase;
        }
        _needsPhaseCalc = false;
    }

    void RichterLFO::_calcFreq(double bpm) {
        // calculate the frequency based on whether tempo sync is active

        const double tempoFreq {(bpm / 60) * (_tempoDenom / _tempoNumer)};

        if (_tempoSyncSwitch) {
            _freq = tempoFreq;
        }

        _freq = Parameters::FREQ.BoundsCheck(_freq);
    }

    void RichterLFO::calcIndexAndScaleInLoop() {
        // calculate the current index within the wave table
        _index = static_cast<int>(static_cast<long>(_samplesProcessed * static_cast<long double>(_currentScale)) % Wavetables::SIZE);

        if ((!CoreMath::compareFloatsEqual(_nextScale, _currentScale)) && (_index == 0)) {
            _currentScale = _nextScale;
            _samplesProcessed = 0;
        }


        // Must provide two possibilities for each index lookup in order to protect the array from
        // being overflowed by the indexOffset, the first if statement uses the standard index
        // lookup while second if statement deals with the overflow possibility

        if ((_index + _indexOffset) < Wavetables::SIZE) {
            _gain = _waveArrayPointer[_index + _indexOffset];
        } else if ((_index + _indexOffset) >= Wavetables::SIZE) {
            _gain = _waveArrayPointer[(_index + _indexOffset) % Wavetables::SIZE];
        }

        _samplesProcessed++;
    }

    double RichterLFO::_getNextOutputImpl(double /*inSample*/) {
        calcIndexAndScaleInLoop();

        // Get the mod amount to use, divide by 2 to reduce range to -0.5:0.5
        const double modAmount {_modulationSource != nullptr ? _modulationSource->getNextOutput(0) / 2 : 0};

        _calcFreqInLoop(modAmount);

        // Calculate the depth value after modulation
        const double depth {Parameters::DEPTH.BoundsCheck(
            _rawDepth + (_depthMod * Parameters::DEPTH.maxValue * modAmount)
        )};

        if (_bypassSwitch) {
            // Produce a value in the range -1:1, invert if needed
            return (_gain * depth) * (_invertSwitch ? -1 : 1);
        } else {
            return 0;
        }
    }

    void RichterLFO::_calcFreqInLoop(double modAmount) {
        // calculate the frequency based on whether tempo sync or frequency modulation is active

        if (!_tempoSyncSwitch) {
            _freq = _rawFreq + (_freqMod * (Parameters::FREQ.maxValue / 2) * modAmount);
        }

        // Bounds check frequency after the modulation is applied to it
        _freq = Parameters::FREQ.BoundsCheck(_freq);
    }
}
