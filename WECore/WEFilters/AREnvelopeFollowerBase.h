/*
 *  File:       AREnveloperFollowerBase.h
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

#pragma once

#include "AREnvelopeFollowerParameters.h"
#include "ModulationSource.h"

namespace WECore::AREnv {
    /**
     * Base class for an envelope follower with controls for attack and release times.
     */
    class AREnvelopeFollowerBase : public ModulationSource<double> {
    public:
        AREnvelopeFollowerBase() : _attackTimeMs(Parameters::ATTACK_MS.defaultValue),
                                   _releaseTimeMs(Parameters::RELEASE_MS.defaultValue) {
            // call this here rather than setting it in initialiser list so that the coefficients get
            // setup
            reset();
            setSampleRate(44100);
        }

        virtual ~AREnvelopeFollowerBase() override = default;

        /** @name Setter Methods */
        /** @{ */

        /**
         * Sets the sample rate the envelope will operate at.
         * It is recommended that you call this at some point before calling clockUpdateEnvelope.
         *
         * @param[in]   sampleRate  The sample rate in Hz
         */
        inline void setSampleRate(double sampleRate);

        /**
         * Sets the attack time of the envelope.
         *
         * @see ATTACK_MS for valid values
         *
         * @param[in]   time    Attack time in milliseconds
         */
        inline void setAttackTimeMs(double time);

        /**
         * Sets the release time of the envelope.
         *
         * @see RELEASE_MS for valid values
         *
         * @param[in]   time    Release time in milliseconds
         */
        inline void setReleaseTimeMs(double time);

        /** @} */

        /** @name Getter Methods */
        /** @{ */

        /**
         * @see     setAttackTimeMs
         */
        double getAttackTimeMs() const { return _attackTimeMs; }

        /**
         * @see     setReleaseTimeMs
         */
        double getReleaseTimeMs() const { return _releaseTimeMs; }
        /** @} */

        /**
         * Resets the envelope state.
         */
        void _resetImpl() override { _envVal = 0; }

    protected:
        double _envVal;

        double _attackTimeMs;
        double _releaseTimeMs;

        double _attackCoef;
        double _releaseCoef;

        double _sampleRate;

        double _calcCoef(double timeMs) {
            return exp(log(0.01) / (timeMs * _sampleRate * 0.001));
        }
    };

    void AREnvelopeFollowerBase::setSampleRate(double sampleRate) {
        _sampleRate = sampleRate;
        _attackCoef = _calcCoef(_attackTimeMs);
        _releaseCoef = _calcCoef(_releaseTimeMs);
    }

    void AREnvelopeFollowerBase::setAttackTimeMs(double time) {
        _attackTimeMs = WECore::AREnv::Parameters::ATTACK_MS.BoundsCheck(time);
        _attackCoef = _calcCoef(_attackTimeMs);
    }

    void AREnvelopeFollowerBase::setReleaseTimeMs(double time) {
        _releaseTimeMs = WECore::AREnv::Parameters::RELEASE_MS.BoundsCheck(time);
        _releaseCoef = _calcCoef(_releaseTimeMs);
    }
}
