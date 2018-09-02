/*
 *  File:       TPTSVFilter.h
 *
 *  Version:    1.0.0
 *
 *  Created:    22/12/2016
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

#include "General/CoreMath.h"
#include "WEFilters/TPTSVFilterParameters.h"

namespace WECore::TPTSVF {
    /**
     * A state variable filter from a topology-preserving transform.
     *
     * To use this class, simply call reset, and the process methods as necessary, using the provided
     * getter and setter methods to manipulate parameters.
     *
     * Internally relies on the parameters provided in TPTSVFilterParameters.h
     *
     * Based on a talk given by Ivan Cohen: https://www.youtube.com/watch?v=esjHXGPyrhg
     */
    template <typename T>
    class TPTSVFilter {
        static_assert(std::is_floating_point<T>::value,
                      "Must be provided with a floating point template type");

    public:
        TPTSVFilter() : _sampleRate(44100),
                        _cutoffHz(Parameters::CUTOFF.defaultValue),
                        _Q(Parameters::Q.defaultValue),
                        _gain(Parameters::GAIN.defaultValue),
                        _s1(0),
                        _s2(0),
                        _mode(Parameters::FILTER_MODE.BYPASS) {}
        
        TPTSVFilter(const TPTSVFilter& other) = default;
        virtual ~TPTSVFilter() = default;
        
        /**
         * Applies the filtering to a buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect. 
         *
         * @param[out]  inSamples   Pointer to the first sample of the left channel's buffer
         * @param[in]   numSamples  Number of samples in the buffer
         */
        void processBlock(T* inSamples, size_t numSamples);
        
        /**
         * Resets filter coefficients.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        void reset() {
            _s1 = 0;
            _s2 = 0;
        }
        
        /** @name Getter Methods */
        /** @{ */
        
        int getMode() const {return _mode;}
        double getCutoff() const {return _cutoffHz;}
        double getQ() const {return _Q;}
        double getGain() const {return _gain;}
        
        /** @} */
        
        /** @name Setter Methods */
        /** @{ */

        void setMode(int val) {_mode = Parameters::FILTER_MODE.BoundsCheck(val);}
        void setCutoff(double val) {_cutoffHz = Parameters::CUTOFF.BoundsCheck(val);}
        void setQ(double val) {_Q = Parameters::Q.BoundsCheck(val);}
        void setGain(double val) {_gain = Parameters::GAIN.BoundsCheck(val);}
        void setSampleRate(double val) {_sampleRate = val;}
        
        /** @} */

        
    private:
        double  _sampleRate,
                _cutoffHz,
                _Q,
                _gain,
                _s1,
                _s2;
        
        int _mode;
    };

    template <typename T>
    void TPTSVFilter<T>::processBlock(T* inSamples, size_t numSamples) {
        
        if (_mode != Parameters::FILTER_MODE.BYPASS) {
            const T g {std::tan(CoreMath::DOUBLE_PI * _cutoffHz / _sampleRate)};
            const T h {1.0f / (1 + g / _Q + g * g)};
            
            for (size_t idx {0}; idx < numSamples; idx++) {
                const T sample {inSamples[idx]};
                
                const T yH {h * (sample - (1.0f / _Q + g) * _s1 - _s2)};
                
                const T yB {g * yH + _s1};
                _s1 = g * yH + yB;
                
                const T yL {g * yB + _s2};
                _s2 = g * yB + yL;
                
                switch (_mode) {
                    case Parameters::ModeParameter::PEAK:
                        inSamples[idx] = yB * _gain;
                        break;
                        
                    case Parameters::ModeParameter::HIGHPASS:
                        inSamples[idx] = yH * _gain;
                        break;
                        
                    default:
                        inSamples[idx] = yL * _gain;
                        break;
                }
            }
        }
    }
}
