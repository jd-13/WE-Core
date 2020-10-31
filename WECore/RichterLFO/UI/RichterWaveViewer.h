/*
 *  File:       RichterWaveViewer.h
 *
 *  Version:    2.0.0
 *
 *  Created:    16/07/2020
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
 *  along with the WECore.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "JuceHeader.h"
#include "RichterLFO/RichterWavetables.h"

namespace WECore::Richter {

    class WaveViewer : public Component {
    public:
        WaveViewer() : _waveArrayPointer(nullptr), _depth(0), _phaseShift(0), _isInverted(false) {}

        inline void setWave(const double* pointer, double depth, int phaseShift, bool isInverted);

        inline virtual void paint(Graphics& g);

        void stop() { _waveArrayPointer = nullptr; }

        enum ColourIds
        {
            highlightColourId = 0x1201201
        };

    private:
        const double* _waveArrayPointer;
        double _depth;
        int _phaseShift;
        bool _isInverted;
    };

    void WaveViewer::setWave(const double* pointer, double depth, int phaseShift, bool isInverted) {
        _waveArrayPointer = pointer;
        _depth = depth;
        _phaseShift = phaseShift;
        _isInverted = isInverted;
    }

    void WaveViewer::paint(Graphics &g) {

        // Down sample the wave array
        constexpr int NUM_SAMPLES {25};
        constexpr float SCALE {0.4};
        constexpr float MARGIN { (1 - SCALE) / 2 };
        const float INCREMENT {static_cast<float>(Wavetables::SIZE) / NUM_SAMPLES};

        if (_waveArrayPointer != nullptr) {
            Path p;

            for (size_t idx {0}; idx < NUM_SAMPLES; idx++) {
                // Calculate the index of the sample accounting for downsampling and phase shift
                const int sampleIdx {(
                    (static_cast<int>(idx * INCREMENT + _phaseShift) % Wavetables::SIZE)
                )};

                // Get the sample for this value
                const double sample {_waveArrayPointer[sampleIdx] * _depth * (_isInverted ? -1 : 1)};

                // Invert the wave and scale to the height of this component
                const double sampleX {(static_cast<double>(idx) / NUM_SAMPLES) * getWidth()};
                const double sampleY = (0.5 - sample) * getHeight() * SCALE + getHeight() * MARGIN;

                // Add it to the path
                if (idx == 0) {
                    p.startNewSubPath(0, sampleY);
                } else {
                    p.lineTo(sampleX, sampleY);
                }
            }

            g.setColour(findColour(highlightColourId));
            g.strokePath(p, PathStrokeType(3.0f));
        }
    }
}
