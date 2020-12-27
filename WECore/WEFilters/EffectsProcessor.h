/*
 *  File:       EffectsProcessor.h
 *
 *  Created:    03/12/2020
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

#include <cassert>
#include <unordered_map>
#include <vector>
namespace WECore {

    /**
     * Provides a standard interface for effects which process a buffer of samples.
     */
    template <typename SampleType>
    class EffectsProcessor {
        static_assert(std::is_floating_point<SampleType>::value,
                      "Must be provided with a floating point template type");

    public:
        EffectsProcessor() = default;
        virtual ~EffectsProcessor() = default;

        /**
         * Override these as required by the processor.
         *
         * Any combination can be overidden as needed, but unimplemented methods must not be called.
         */
        virtual void process1in1out(SampleType* /*inSamples*/, size_t /*numSamples*/) { assert(false); }
        virtual void process1in2out(SampleType* /*inSamplesLeft*/, SampleType* /*inSamplesRight*/, size_t /*numSamples*/) { assert(false); }
        virtual void process2in2out(SampleType* /*inSamplesLeft*/, SampleType* /*inSamplesRight*/, size_t /*numSamples*/) { assert(false); }

        /**
         * Resets the internal state of the processor.
         *
         * Override this to reset everything as necessary.
         */
        inline virtual void reset() {}
    };
}
