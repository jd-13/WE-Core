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

#include "RichterLFOBase.h"

namespace WECore::Richter {

    class RichterLFOPair;

    /**
     * Provides and LFO with: depth, rate, tempo sync, phase, wave shape, and phase sync
     * controls, plus additional functionality to allow the depth and rate controls to be
     * modulated by an external source, with internal controls of the for the depth of this
     * modulation.
     *
     * This LFO oscillates between 0 and 1, and so is useful for applying amplitude
     * modulation directly to an audio signal.
     *
     * To use, you simply need to call reset, prepareForNextBuffer, and calcGainInLoop
     * as necessary (see their descriptions for details), and use the provided getter
     * and setter methods to manipulate parameters.
     *
     * Completes the implementation of RichterLFO.
     */
    class RichterLFO : public RichterLFOBase {

    public:

        /**
         * Generates the wave tables on initialsation, while running gain values
         * are simply looked up from these wave tables.
         *
         * Also initialises parameters (that are not part of RichterLFOBase) to default values.
         */
        inline RichterLFO();

        virtual ~RichterLFO() = default;

        friend class RichterLFOPair;

        /** @name Getter Methods */
        /** @{ */

        double getRawDepth() { return _rawDepth; }

        double getDepthMod() { return _depthMod; }

        double getRawFreq() { return _rawFreq; }

        double getFreqMod() { return _freqMod; }

        /** @} */

        /** @name Setter Methods */
        /** @{ */

        void setRawFreq(double val) { _rawFreq = Parameters::FREQ.BoundsCheck(val); }

        void setFreqMod(double val) { _freqMod = Parameters::FREQMOD.BoundsCheck(val); }

        void setRawDepth(double val) { _rawDepth = Parameters::DEPTH.BoundsCheck(val); }

        void setDepthMod(double val) { _depthMod = Parameters::DEPTHMOD.BoundsCheck(val); }

        /** @} */

        /**
         * Use this in your processing loop. Returns a gain value which is intended to be
         * multiplied with a single sample to apply the tremolo effect.
         *
         * Note: Calling this method will advance the oscillators internal counters by one
         *       sample. Calling this method will return a different value each time.
         *
         * @param   modBypassSwitch     The state of the modulation oscillator. If set to true,
         *                              the effect of modGain will be applied.
         * @param   modGain             The gain output from the modulation oscillator.
         *
         * @return  The value of the LFO's output at this moment, a value between 0 and 1.
         */
        inline double calcGainInLoop(int modBypassSwitch, double modGain);

        RichterLFO operator= (RichterLFO& other) = delete;
        RichterLFO(RichterLFO&) = delete;

    private:

        double  _rawFreq,
                _freqMod,
                _rawDepth,
                _depthMod;

        /**
         * Applies frequency modulation to the oscillator. Performed in the processing
         * loop so that the frequency can be updated before processing each sample.
         *
         * @param   modBypassSwitch     The state of the modulation oscillator. If set to true,
         *                              the effect of modGain will be applied.
         * @param   modGain             The gain output from the modulation oscillator.
         */
        inline void _calcFreqInLoop(int modBypassSwitch, double modGain);

        /**
         * Applies depth modulation to the oscillator. Performed in the processing
         * loop so that the frequency can be updated before processing each sample.
         *
         * args: modBypassSwitch   The state of the modulation oscillator. Determines
         *                         whether modGain is applied to the calculation
         *       modGain           The gain output from the modulation oscillator
         */
        inline void _calcDepthInLoop(int modBypassSwitch, double modGain);


        /**
         * Calculates the gain value to be applied to a signal which the oscillator
         * is operating on. Outputs a value between 0 and 1. Always outputs 1 if bypassed.
         *
         * args: modBypassSwitch   The state of the modulation oscillator. Determines
         *                         whether modGain is applied to the calculation
         *       modGain           The gain output from the modulation oscillator
         */
        inline double _calcGain(int modBypassSwitch, double modGain);
    };

    RichterLFO::RichterLFO() : _rawFreq(Parameters::FREQ.defaultValue),
                               _freqMod(Parameters::FREQMOD.defaultValue),
                               _rawDepth(Parameters::DEPTH.defaultValue),
                               _depthMod(Parameters::DEPTHMOD.defaultValue) {
    }

    double RichterLFO::calcGainInLoop(int modBypassSwitch, double modGain) {
        calcIndexAndScaleInLoop();
        return _calcGain(modBypassSwitch, modGain);
    }

    void RichterLFO::_calcFreqInLoop(int modBypassSwitch, double modGain) {
        // calculate the frequency based on whether tempo sync or frequency modulation is active

        if (!_tempoSyncSwitch) {
            if (modBypassSwitch) {
                _freq = _rawFreq + (_freqMod * (Parameters::FREQ.maxValue / 2) * modGain);
            } else {
                _freq = _rawFreq;
            }
        }

        // Bounds check frequency after the modulation is applied to it
        _freq = Parameters::FREQ.BoundsCheck(_freq);

    }

    void RichterLFO::_calcDepthInLoop(int modBypassSwitch, double modGain) {
        // Check whether MOD oscs are activated and apply depth parameter modulation accordingly

        if (modBypassSwitch) {
            _depth = _rawDepth + (_depthMod * Parameters::DEPTH.maxValue * modGain);
        } else {
            _depth = _rawDepth;
        }

        _depth = Parameters::DEPTH.BoundsCheck(_depth);

    }

    double RichterLFO::_calcGain(int modBypassSwitch, double modGain) {
        _calcFreqInLoop(modBypassSwitch, modGain);
        _calcDepthInLoop(modBypassSwitch, modGain);

        if (_bypassSwitch) {
            // Convert range from -1:1 to 0:1
            return 1 + (_gain * _depth / 2 - 0.5) + (1 - _depth) / 2;
        } else {
            return 1;
        }
    }
}
