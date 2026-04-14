/*
 *  File:       StepSequencer.h
 *
 *  Created:    04/04/2026
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

#include <vector>
#include <cmath>
#include <algorithm>
#include "General/ParameterDefinition.h"
#include "WEFilters/ModulationSource.h"

namespace WECore::StepSeq {

    static constexpr int DEFAULT_NUM_STEPS {16};

    enum class StepShape : int {
        FLAT = 1,
        RAMP = 2,
        EXP = 3,
        REV_EXP = 4
    };

    namespace Parameters {
        const ParameterDefinition::RangedParameter<double> DEPTH(0, 1, 0.5);
        const ParameterDefinition::RangedParameter<double> FREQ(0.05, 20, 2);
        const ParameterDefinition::RangedParameter<int>    TEMPONUMER(1, 32, 1);
        const ParameterDefinition::RangedParameter<int>    TEMPODENOM(1, 32, 1);
        const ParameterDefinition::RangedParameter<double> STEP_VALUE(0, 1, 0);
        const ParameterDefinition::RangedParameter<int>    STEP_REPEAT(1, 8, 1);

        constexpr bool TEMPOSYNC_OFF = false,
                       TEMPOSYNC_ON = true,
                       TEMPOSYNC_DEFAULT = TEMPOSYNC_OFF;
    }

    struct Step {
        double value {Parameters::STEP_VALUE.defaultValue};
        StepShape shape {StepShape::FLAT};
        bool reverse {false};
        int repeat {Parameters::STEP_REPEAT.defaultValue};
    };

    struct Pattern {
        std::vector<Step> steps;

        Pattern(int numSteps = DEFAULT_NUM_STEPS) : steps(numSteps) { }
    };

    class StepSequencer : public WECore::ModulationSource<double> {
    public:
        StepSequencer() : _tempoSyncSwitch(Parameters::TEMPOSYNC_DEFAULT),
                          _needsSeekOffsetCalc(true),
                          _tempoNumer(Parameters::TEMPONUMER.defaultValue),
                          _tempoDenom(Parameters::TEMPODENOM.defaultValue),
                          _rawFreq(Parameters::FREQ.defaultValue),
                          _modulatedFreqValue(0),
                          _rawDepth(Parameters::DEPTH.defaultValue),
                          _modulatedDepthValue(0),
                          _sampleRate(44100),
                          _bpm(120),
                          _currentStep(0),
                          _samplePosition(0),
                          _patterns(1) {
        }

        virtual ~StepSequencer() override = default;

        /** @name Getter Methods */
        /** @{ */
        bool getTempoSyncSwitch() const { return _tempoSyncSwitch; }
        int getTempoNumer() const { return _tempoNumer; }
        int getTempoDenom() const { return _tempoDenom; }
        double getFreq() const { return _rawFreq; }
        double getModulatedFreqValue() const { return _modulatedFreqValue; }
        double getDepth() const { return _rawDepth; }
        double getModulatedDepthValue() const { return _modulatedDepthValue; }
        int getCurrentStep() const { return _currentStep; }
        /** @} */

        /** @name Setter Methods */
        /** @{ */
        void setTempoSyncSwitch(bool val) { _tempoSyncSwitch = val; }
        void setTempoNumer(int val) { _tempoNumer = Parameters::TEMPONUMER.BoundsCheck(val); }
        void setTempoDenom(int val) { _tempoDenom = Parameters::TEMPODENOM.BoundsCheck(val); }
        void setFreq(double val) { _rawFreq = Parameters::FREQ.BoundsCheck(val); }
        void setDepth(double val) { _rawDepth = Parameters::DEPTH.BoundsCheck(val); }
        void setSampleRate(double val) { _sampleRate = val; }
        /** @} */

        inline bool addFreqModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool removeFreqModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool setFreqModulationAmount(size_t index, double amount);
        void setFreqModulationSources(std::vector<ModulationSourceWrapper<double>> sources) { _freqModulationSources = sources; }
        std::vector<ModulationSourceWrapper<double>> getFreqModulationSources() const { return _freqModulationSources; }

        inline bool addDepthModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool removeDepthModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool setDepthModulationAmount(size_t index, double amount);
        void setDepthModulationSources(std::vector<ModulationSourceWrapper<double>> sources) { _depthModulationSources = sources; }
        std::vector<ModulationSourceWrapper<double>> getDepthModulationSources() const { return _depthModulationSources; }

        /** @name Pattern Access */
        /** @{ */
        int getNumPatterns() const { return static_cast<int>(_patterns.size()); }

        const Pattern& getPattern(int patternIndex) const { return _patterns[patternIndex]; }
        /** @} */

        /** @name Step Management */
        /** @{ */
        void addStep(int patternIndex) {
            if (patternIndex >= _patterns.size()) {
                return;
            }
            _patterns[patternIndex].steps.emplace_back();
        }

        void removeStep(int patternIndex) {
            if (patternIndex >= _patterns.size()) {
                return;
            }

            Pattern& pattern = _patterns[patternIndex];
            if (pattern.steps.size() > 1) {
                pattern.steps.pop_back();

                // Ensure current step index is valid after removing a step
                if (_currentStep >= static_cast<int>(pattern.steps.size())) {
                    _currentStep = 0;
                    _samplePosition = 0;
                }
            }
        }

        int getNumSteps(int patternIndex) const {
            if (patternIndex >= _patterns.size()) {
                return 0;
            }
            return static_cast<int>(_patterns[patternIndex].steps.size());
        }
        /** @} */

        /** @name Per-Step Parameters */
        /** @{ */
        void setStepValue(int patternIndex, int stepIndex, double value) {
            if (patternIndex >= _patterns.size() || stepIndex >= static_cast<int>(_patterns[patternIndex].steps.size())) {
                return;
            }
            _patterns[patternIndex].steps[stepIndex].value = Parameters::STEP_VALUE.BoundsCheck(value);
        }

        void setStepShape(int patternIndex, int stepIndex, StepShape shape) {
            if (patternIndex >= _patterns.size() || stepIndex >= static_cast<int>(_patterns[patternIndex].steps.size())) {
                return;
            }
            _patterns[patternIndex].steps[stepIndex].shape = shape;
        }

        void setStepReverse(int patternIndex, int stepIndex, bool reverse) {
            if (patternIndex >= _patterns.size() || stepIndex >= static_cast<int>(_patterns[patternIndex].steps.size())) {
                return;
            }
            _patterns[patternIndex].steps[stepIndex].reverse = reverse;
        }

        void setStepRepeat(int patternIndex, int stepIndex, int repeat) {
            if (patternIndex >= _patterns.size() || stepIndex >= static_cast<int>(_patterns[patternIndex].steps.size())) {
                return;
            }
            _patterns[patternIndex].steps[stepIndex].repeat = Parameters::STEP_REPEAT.BoundsCheck(repeat);
        }

        double getStepValue(int patternIndex, int stepIndex) const {
            if (patternIndex >= _patterns.size() || stepIndex >= static_cast<int>(_patterns[patternIndex].steps.size())) {
                return 0;
            }
            return _patterns[patternIndex].steps[stepIndex].value;
        }

        StepShape getStepShape(int patternIndex, int stepIndex) const {
            if (patternIndex >= _patterns.size() || stepIndex >= static_cast<int>(_patterns[patternIndex].steps.size())) {
                return StepShape::FLAT;
            }

            return _patterns[patternIndex].steps[stepIndex].shape;
        }

        bool getStepReverse(int patternIndex, int stepIndex) const {
            if (patternIndex >= _patterns.size() || stepIndex >= static_cast<int>(_patterns[patternIndex].steps.size())) {
                return false;
            }
            return _patterns[patternIndex].steps[stepIndex].reverse;
        }

        int getStepRepeat(int patternIndex, int stepIndex) const {
            if (patternIndex >= _patterns.size() || stepIndex >= static_cast<int>(_patterns[patternIndex].steps.size())) {
                return 1;
            }
            return _patterns[patternIndex].steps[stepIndex].repeat;
        }

        /** @} */

        /**
         * Prepares for processing the next buffer of samples.
         *
         * @param   bpm   Current bpm of the host
         */
        void prepareForNextBuffer(double bpm, double timeInSeconds) {
            _bpm = bpm;
            _calcPhaseOffset(timeInSeconds);
        }

        StepSequencer operator=(StepSequencer& other) = delete;

        /**
         * Returns a pointer to a deep copy of this instance, including all state. The caller is
         * responsible for managing the lifetime of the returned object.
         */
        StepSequencer* clone() const {
            return new StepSequencer(*this);
        }

    protected:
        bool _tempoSyncSwitch,
             _needsSeekOffsetCalc;

        int _tempoNumer,
            _tempoDenom;

        double _rawFreq,
               _modulatedFreqValue,
               _rawDepth,
               _modulatedDepthValue,
               _sampleRate,
               _bpm;

        int _currentStep;
        double _samplePosition;

        std::vector<Pattern> _patterns;

        std::vector<ModulationSourceWrapper<double>> _freqModulationSources,
                                                     _depthModulationSources;

        inline double _getNextOutputImpl(double inSample) override;
        inline void _resetImpl() override;

        inline void _calcPhaseOffset(double timeInSeconds);
        inline double _calcBaseStepDuration() const;
        inline double _calcShapedValue(double t, StepShape shape) const;

    private:
        // Used when cloning
        StepSequencer(const StepSequencer& other) {
            _tempoSyncSwitch = other._tempoSyncSwitch;
            _needsSeekOffsetCalc = other._needsSeekOffsetCalc;

            _tempoNumer = other._tempoNumer;
            _tempoDenom = other._tempoDenom;
            _rawFreq = other._rawFreq;
            _modulatedFreqValue = other._modulatedFreqValue;
            _rawDepth = other._rawDepth;
            _modulatedDepthValue = other._modulatedDepthValue;
            _sampleRate = other._sampleRate;
            _bpm = other._bpm;

            _currentStep = other._currentStep;
            _samplePosition = other._samplePosition;

            _patterns = other._patterns;

            _freqModulationSources = other._freqModulationSources;
            _depthModulationSources = other._depthModulationSources;

            _cachedOutput = other._cachedOutput;
        }
    };

    void StepSequencer::_resetImpl() {
        _needsSeekOffsetCalc = true;
        _currentStep = 0;
        _samplePosition = 0;
    }

    void StepSequencer::_calcPhaseOffset(double timeInSeconds) {
        if (!_needsSeekOffsetCalc) {
            return;
        }

        _needsSeekOffsetCalc = false;

        const Pattern& pattern = _patterns[0];
        const int numSteps = static_cast<int>(pattern.steps.size());
        const double baseDuration = _calcBaseStepDuration();

        // Calculate total cycle length in samples
        const double totalCycleSamples = baseDuration * numSteps;

        // Convert playhead time to samples and find position within the cycle
        const double positionInSamples = std::fmod(timeInSeconds * _sampleRate, totalCycleSamples);

        // Walk through steps to find which step and position we should be at
        double accumulated = 0;
        for (int i = 0; i < numSteps; i++) {
            const double stepDuration = baseDuration;

            if (accumulated + stepDuration > positionInSamples) {
                _currentStep = i;
                _samplePosition = positionInSamples - accumulated;
                return;
            }

            accumulated += stepDuration;
        }

        // Shouldn't reach here, but fall back to start
        _currentStep = 0;
        _samplePosition = 0;
    }

    double StepSequencer::_calcBaseStepDuration() const {
        if (_tempoSyncSwitch) {
            return _sampleRate * (static_cast<double>(_tempoNumer) / _tempoDenom) * (60.0 / _bpm);
        } else {
            return _sampleRate / _rawFreq;
        }
    }

    double StepSequencer::_calcShapedValue(double t, StepShape shape) const {
        switch (shape) {
            case StepShape::FLAT:    return 1.0;
            case StepShape::RAMP:    return t;
            case StepShape::EXP:     return t * t;
            case StepShape::REV_EXP: return 1.0 - t * t;
            default:                 return 1.0;
        }
    }

    double StepSequencer::_getNextOutputImpl(double /*inSample*/) {
        const double freqModValue {calcModValue(_freqModulationSources) / 2};
        const double depthModValue {calcModValue(_depthModulationSources) / 2};

        // Calculate modulated frequency
        const double freq {Parameters::FREQ.BoundsCheck(
            _rawFreq + ((Parameters::FREQ.maxValue / 2) * freqModValue)
        )};
        _modulatedFreqValue = freq;

        const Pattern& pattern = _patterns[0];
        const int numSteps = static_cast<int>(pattern.steps.size());

        const double currentStepDuration = _tempoSyncSwitch
            ? _sampleRate * (static_cast<double>(_tempoNumer) / _tempoDenom) * (60.0 / _bpm)
            : _sampleRate / _modulatedFreqValue;

        // Advance position by one sample and handle step transitions. The while loop
        // handles the case where currentStepDuration is shorter than one sample, advancing
        // multiple steps if needed.
        _samplePosition += 1.0;
        while (_samplePosition >= currentStepDuration) {
            _samplePosition -= currentStepDuration;
            _currentStep = (_currentStep + 1) % numSteps;
        }

        const Step& step = pattern.steps[_currentStep];

        // Calculate position within current step (0..1), cycling repeat times
        const double repetitionDuration = currentStepDuration / step.repeat;
        const double t = std::fmod(_samplePosition, repetitionDuration) / repetitionDuration;

        // Apply shape (reverse flips the interpolation direction)
        const double tShaped = step.reverse ? 1.0 - t : t;
        const double shaped = _calcShapedValue(tShaped, step.shape);

        // Calculate modulated depth
        const double depth {Parameters::DEPTH.BoundsCheck(
            _rawDepth + (Parameters::DEPTH.maxValue * depthModValue)
        )};
        _modulatedDepthValue = depth;

        // Calculate output (always unipolar: step values are 0..1, output is 0..depth)
        return step.value * shaped * _modulatedDepthValue;
    }

    bool StepSequencer::addFreqModulationSource(std::shared_ptr<ModulationSource> source) {
        // Check if the source is already in the list
        for (const ModulationSourceWrapper<double>& existingSource : _freqModulationSources) {
            if (existingSource.source == source) {
                return false;
            }
        }

        _freqModulationSources.push_back({source, 0});
        return true;
    }

    bool StepSequencer::removeFreqModulationSource(std::shared_ptr<ModulationSource> source) {
        for (auto it = _freqModulationSources.begin(); it != _freqModulationSources.end(); it++) {
            if ((*it).source == source) {
                _freqModulationSources.erase(it);
                return true;
            }
        }
        return false;
    }

    bool StepSequencer::setFreqModulationAmount(size_t index, double amount) {
        if (index < _freqModulationSources.size()) {
            _freqModulationSources[index].amount = amount;
            return true;
        }
        return false;
    }

    bool StepSequencer::addDepthModulationSource(std::shared_ptr<ModulationSource> source) {
        // Check if the source is already in the list
        for (const ModulationSourceWrapper<double>& existingSource : _depthModulationSources) {
            if (existingSource.source == source) {
                return false;
            }
        }

        _depthModulationSources.push_back({source, 0});
        return true;
    }

    bool StepSequencer::removeDepthModulationSource(std::shared_ptr<ModulationSource> source) {
        for (auto it = _depthModulationSources.begin(); it != _depthModulationSources.end(); it++) {
            if ((*it).source == source) {
                _depthModulationSources.erase(it);
                return true;
            }
        }
        return false;
    }

    bool StepSequencer::setDepthModulationAmount(size_t index, double amount) {
        if (index < _depthModulationSources.size()) {
            _depthModulationSources[index].amount = amount;
            return true;
        }
        return false;
    }
}
