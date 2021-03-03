/*
 *	File:		CoreAudioProcessor.h
 *
 *	Version:	1.0.0
 *
 *	Created:	10/06/2017
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
 *
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "General/ParameterDefinition.h"

namespace WECore::JUCEPlugin {

    /**
     * This class provides basic functionality that is commonly used by an AudioProcessor in a
     * White Elephant plugin.
     *
     * Classes inheriting from this should:
     *   - Call registerParameter to declare parameters
     */
    class CoreAudioProcessor : public juce::AudioProcessor {
    public:
        CoreAudioProcessor() = default;
        virtual ~CoreAudioProcessor() = default;

        /**
         * Collects the registered parameter values and writes them to XML.
         *
         * Float parameters are written in their normalised 0 to 1 range.
         *
         * Int parameters are written as a float representation of their real (not normalised) value.
         *
         * Bool parameters are written as a float representation of true or false.
         */
        inline void getStateInformation(juce::MemoryBlock& destData) override;

        /**
         * Restores parameter values from previously written XML.
         */
        inline void setStateInformation(const void* data, int sizeInBytes) override;

        /**
         * Adds a listener that will be notified whenever a parameter is changed.
         */
        void addParameterChangeListener(juce::ChangeListener* listener) {
            _parameterBroadcaster.addChangeListener(listener);
        }

        /**
         * Removes a previously added listener.
         */
        void removeParameterChangeListener(juce::ChangeListener* listener) {
            _parameterBroadcaster.removeChangeListener(listener);
        }

    protected:
        /**
         * Used to register parameters.
         *
         * Float parameters are created with the normalised 0 to 1 range.
         *
         * Int parameters are created with their real (not normalised) ranges.
         *
         * (Bool parameters don't have a meaningful range.)
         *
         * The order parameters are registered in determines the order they are persisted to XML
         * and read from XML when restoring previous values. If this order changes between versions
         * then a plugin restoring parameter data from XML written by a previous version will assign
         * values to the wrong parameters.
         */
        /** @{ */
        inline void registerParameter(juce::AudioParameterFloat*& param,
                                      const juce::String& name,
                                      const ParameterDefinition::RangedParameter<double>* range,
                                      float defaultValue,
                                      float precision,
                                      std::function<void(float)> setter);

        inline void registerParameter(juce::AudioParameterInt*& param,
                                      const juce::String& name,
                                      const ParameterDefinition::BaseParameter<int>* range,
                                      int defaultValue,
                                      std::function<void(int)> setter);

        inline void registerParameter(juce::AudioParameterBool*& param,
                                      const juce::String& name,
                                      float defaultValue,
                                      std::function<void(bool)> setter);
        /** @} */

        /**
         * Override this and return a vector of parameter names corresponding to the order that
         * parameters were stored in using the legacy schema.
         */
        virtual std::vector<juce::String> _provideParamNamesForMigration() = 0;

        /**
         * Override this to migrate saved parameter values from normalised to internal.
         */
        virtual void _migrateParamValues(std::vector<float>& paramValues) = 0;

    private:

        // Increment this after changing how parameter states are stored
        static constexpr int PARAMS_SCHEMA_VERSION {1};

        /**
         * Stores a setter and getter for a parameter. Used when persisting parameter values to XML
         * and restoring values from XML.
         */
        struct ParameterInterface {
            juce::String name;
            std::function<float()> getter;
            std::function<void(float)> setter;
        };

        /**
         * Listens for parameter changes and triggers the broadcaster so the changes can be handled
         * by another thread.
         */
        class ParameterBroadcaster : public juce::AudioProcessorParameter::Listener,
                                     public juce::ChangeBroadcaster {
        public:
            ParameterBroadcaster() = default;
            virtual ~ParameterBroadcaster() = default;

            virtual void parameterValueChanged(int /*parameterIndex*/, float /*newValue*/) {
                this->sendChangeMessage();
            }

            virtual void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {}
        };

        ParameterBroadcaster _parameterBroadcaster;

        /**
         * List of parameters in the order they are registered and stored in XML.
         */
        std::vector<ParameterInterface> _paramsList;

        inline juce::String _floatVectorToString(const std::vector<float>& fData) const;

        inline std::vector<float> _stringToFloatVector(const juce::String sFloatCSV) const;

        inline std::unique_ptr<juce::XmlElement> _migrateParameters(
                std::unique_ptr<juce::XmlElement> rootElement);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreAudioProcessor)
    };

    void CoreAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
    {
        // Build the XML
        juce::XmlElement rootElement("Root");

        // Set the XML params version
        rootElement.setAttribute("SchemaVersion", PARAMS_SCHEMA_VERSION);

        // Store the parameters
        juce::XmlElement* paramsElement = rootElement.createNewChildElement("Params");
        for (const ParameterInterface& param : _paramsList) {
            paramsElement->setAttribute(param.name, param.getter());
        }

        copyXmlToBinary(rootElement, destData);
    }

    void CoreAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
    {
        std::unique_ptr<juce::XmlElement> rootElement(getXmlFromBinary(data, sizeInBytes));

        // Parse the XML
        if (rootElement != nullptr) {

            // If state was saved using an old plugin we need to migrate the XML data
            if (rootElement->getIntAttribute("SchemaVersion", 0) < PARAMS_SCHEMA_VERSION) {
                rootElement = _migrateParameters(std::move(rootElement));
            }

            // Iterate through our list of parameters, restoring them from the XML attributes
            juce::XmlElement* paramsElement = rootElement->getChildByName("Params");
            if (paramsElement != nullptr) {
                for (const ParameterInterface& param : _paramsList) {
                    if (paramsElement->hasAttribute(param.name)) {
                        param.setter(paramsElement->getDoubleAttribute(param.name));
                    }
                }
            }
        }
    }

    void CoreAudioProcessor::registerParameter(juce::AudioParameterFloat*& param,
                                               const juce::String& name,
                                               const ParameterDefinition::RangedParameter<double>* range,
                                               float defaultValue,
                                               float precision,
                                               std::function<void(float)> setter) {
        param = new juce::AudioParameterFloat(name, name, {0.0f, 1.0f, precision}, range->InternalToNormalised(defaultValue));

        ParameterInterface interface = {name,
                                        [&param, range]() { return range->NormalisedToInternal(param->get()); },
                                        [setter, range](float val) { setter(range->InternalToNormalised(val)); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    void CoreAudioProcessor::registerParameter(juce::AudioParameterInt*& param,
                                               const juce::String& name,
                                               const ParameterDefinition::BaseParameter<int>* range,
                                               int defaultValue,
                                               std::function<void(int)> setter) {
        param = new juce::AudioParameterInt(name, name, range->minValue, range->maxValue, defaultValue);

        ParameterInterface interface = {name,
                                        [&param]() { return param->get(); },
                                        [setter](float val) { setter(static_cast<int>(val)); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    void CoreAudioProcessor::registerParameter(juce::AudioParameterBool*& param,
                                               const juce::String& name,
                                               float defaultValue,
                                               std::function<void(bool)> setter) {
        param = new juce::AudioParameterBool(name, name, defaultValue);

        ParameterInterface interface = {name,
                                        [&param]() { return param->get(); },
                                        [setter](float val) { setter(static_cast<bool>(val)); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    std::vector<float> CoreAudioProcessor::_stringToFloatVector(const juce::String sFloatCSV) const {
        juce::StringArray tokenizer;
        tokenizer.addTokens(sFloatCSV, ",","");

        std::vector<float> values;

        for (int iii {0}; iii < tokenizer.size(); iii++) {
            values.push_back(tokenizer[iii].getFloatValue());
        }

        return values;
    }

    std::unique_ptr<juce::XmlElement> CoreAudioProcessor::_migrateParameters(std::unique_ptr<juce::XmlElement> rootElement) {
        const int schemaVersion {rootElement->getIntAttribute("SchemaVersion", 0)};

        std::unique_ptr<juce::XmlElement> retVal = std::make_unique<juce::XmlElement>("Root");

        if (schemaVersion == 0) {
            // This is an original parameter schema - parameters are normalised values in a single string

            forEachXmlChildElement((*rootElement), childElement) {
                if (childElement->hasTagName("AllUserParam")) {

                    // Read the values into a float array
                    juce::String sFloatCSV = childElement->getAllSubText();
                    std::vector<float> readParamValues = _stringToFloatVector(sFloatCSV);
                    _migrateParamValues(readParamValues);

                    std::vector<juce::String> paramNames = _provideParamNamesForMigration();

                    juce::XmlElement* paramsElement = retVal->createNewChildElement("Params");

                    for (int idx {0}; idx < paramNames.size(); idx++) {
                        if (idx < readParamValues.size()) {
                            paramsElement->setAttribute(paramNames[idx], readParamValues[idx]);
                        }
                    }
                }
            }
        }

        return retVal;
    }
}
