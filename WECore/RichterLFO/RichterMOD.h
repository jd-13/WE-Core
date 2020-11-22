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

#pragma once

#include "RichterLFOBase.h"

namespace WECore::Richter {

    class RichterLFOPair;

    /**
     * Provides and LFO with: depth, rate, tempo sync, phase, wave shape,
     * and phase sync controls. This LFO oscillates between -0.5 and 0.5, and so
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
        RichterMOD() = default;
        virtual ~RichterMOD() override = default;

        friend class RichterLFOPair;

        RichterMOD operator=(RichterMOD& other) = delete;
        RichterMOD(RichterMOD& other) = delete;

    private:
        /**
         * Note: Calling this method will advance the oscillators internal counters by one
         *       sample. Calling this method will return a different value each time.
         *
         * @return  The value of the LFO's output at this moment, a value between -0.5 and 0.5.
         */
        inline double _getNextOutputImpl(double inSample) override;

        /**
         * Calculates the gain value to be applied to a signal (in this case a
         * parameter of another oscillator) which the oscillator is operating on.
         * Outputs a value between -0.5 and 0.5. Always outputs 0 if bypassed.
         */
        inline double _calcGain();
    };

    double RichterMOD::_getNextOutputImpl(double /*inSample*/) {
        calcIndexAndScaleInLoop();
        return _calcGain();
    }

    double RichterMOD::_calcGain() {
        if (_bypassSwitch) {
            // Divide by 2 to convert range from -1:1 to -0.5:0.5, invert if needed
            return (_gain * _depth) / 2 * (_invertSwitch ? -1 : 1);
        } else {
            return 0;
        }
    }
}
