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

namespace WECore::JUCEPlugin {

    /**
     * This class provides basic functionality that is commonly used by an AudioProcessor in a
     * White Elephant plugin.
     *
     * Classes inheriting from this should:
     *   - Call registerParameter to declare parameters
     */
    class CoreAudioProcessor : public AudioProcessor {
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
        inline void getStateInformation(MemoryBlock& destData) override;

        /**
         * Restores parameter values from previously written XML.
         */
        inline void setStateInformation(const void* data, int sizeInBytes) override;

        /**
         * Adds a listener that will be notified whenever a parameter is changed.
         */
        void addParameterChangeListener(ChangeListener* listener) {
            _parameterBroadcaster.addChangeListener(listener);
        }

        /**
         * Removes a previously added listener.
         */
        void removeParameterChangeListener(ChangeListener* listener) {
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
        inline void registerParameter(AudioParameterFloat*& param,
                                      const String& name,
                                      float defaultValue,
                                      std::function<void(float)> setter);

        inline void registerParameter(AudioParameterInt*& param,
                                      const String& name,
                                      const ParameterDefinition::BaseParameter<int>* range,
                                      int defaultValue,
                                      std::function<void(int)> setter);

        inline void registerParameter(AudioParameterBool*& param,
                                      const String& name,
                                      float defaultValue,
                                      std::function<void(bool)> setter);
        /** @} */

    private:

        /**
         * Stores a setter and getter for a parameter. Used when persisting parameter values to XML
         * and restoring values from XML.
         */
        struct ParameterInterface {
            std::function<float()> getter;
            std::function<void(float)> setter;
        };

        /**
         * Listens for parameter changes and triggers the broadcaster so the changes can be handled
         * by another thread.
         */
        class ParameterBroadcaster : public AudioProcessorParameter::Listener,
                                     public ChangeBroadcaster {
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

        inline String _floatVectorToString(const std::vector<float>& fData) const;

        inline std::vector<float> _stringToFloatVector(const String sFloatCSV) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreAudioProcessor)
    };

    void CoreAudioProcessor::getStateInformation(MemoryBlock& destData)
    {
        // Compile the list of parameter values
        std::vector<float> paramValues;
        for (const ParameterInterface& param : _paramsList) {
            paramValues.push_back(param.getter());
        }

        // Build the XML
        XmlElement root("Root");
        XmlElement *el = root.createNewChildElement("AllUserParam");

        el->addTextElement(String(_floatVectorToString(paramValues)));
        copyXmlToBinary(root, destData);
    }

    void CoreAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
    {
        std::unique_ptr<XmlElement> pRoot(getXmlFromBinary(data, sizeInBytes));
        std::vector<float> readParamValues;

        // Parse the XML
        if (pRoot != NULL) {
            forEachXmlChildElement((*pRoot), pChild) {
                if (pChild->hasTagName("AllUserParam")) {
                    
                    // Read the values into a float array
                    String sFloatCSV = pChild->getAllSubText();
                    const std::vector<float> readParamValues = _stringToFloatVector(sFloatCSV);
                    
                    // Pass each value read from XML to a setter, being careful not to go out of
                    // range on either of the vectors
                    //
                    // For this to work it relies the order not changing between plugin versions,
                    // otherwise parameter values written by an old version will be assigned to the
                    // wrong parameters in a new version (new parameters are ok though)
                    for (int idx {0}; idx < _paramsList.size(); idx++) {
                        if (idx < readParamValues.size()) {
                            _paramsList[idx].setter(readParamValues[idx]);
                        }
                    }
                }
            }
        }
    }

    void CoreAudioProcessor::registerParameter(AudioParameterFloat*& param,
                                               const String& name,
                                               float defaultValue,
                                               std::function<void(float)> setter) {
        param = new AudioParameterFloat(name, name, 0.0f, 1.0f, defaultValue);

        ParameterInterface interface = {[&param]() { return param->get(); },
                                        setter};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    void CoreAudioProcessor::registerParameter(AudioParameterInt*& param,
                                               const String& name,
                                               const ParameterDefinition::BaseParameter<int>* range,
                                               int defaultValue,
                                               std::function<void(int)> setter) {
        param = new AudioParameterInt(name, name, range->minValue, range->maxValue, defaultValue);

        ParameterInterface interface = {[&param]() { return param->get(); },
                                        [setter](float val) { setter(static_cast<int>(val)); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    void CoreAudioProcessor::registerParameter(AudioParameterBool*& param,
                                               const String& name,
                                               float defaultValue,
                                               std::function<void(bool)> setter) {
        param = new AudioParameterBool(name, name, defaultValue);

        ParameterInterface interface = {[&param]() { return param->get(); },
                                        [setter](float val) { setter(static_cast<bool>(val)); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    String CoreAudioProcessor::_floatVectorToString(const std::vector<float>& fData) const {
        String result {""};

        if (fData.size() < 1) {
            return result;
        }

        for (int iii {0}; iii < (fData.size() - 1); iii++) {
            result << String(fData[iii])<<",";
        }

        result << String(fData[fData.size() - 1]);

        return result;
    }

    std::vector<float> CoreAudioProcessor::_stringToFloatVector(const String sFloatCSV) const {
        StringArray tokenizer;
        tokenizer.addTokens(sFloatCSV, ",","");
        
        std::vector<float> values;

        for (int iii {0}; iii < tokenizer.size(); iii++) {
            values.push_back(tokenizer[iii].getFloatValue());
        }

        return values;
    }
}
