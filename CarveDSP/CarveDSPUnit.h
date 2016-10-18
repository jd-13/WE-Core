/*
 *	File:		CarveDSPUnit.h
 *
 *	Version:	2.0.0
 *
 *	Created:	09/09/2015
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


#ifndef Carve_CarveDSPUnit_h
#define Carve_CarveDSPUnit_h

#define _USE_MATH_DEFINES

#include "math.h"
#include "CarveParameters.h"


/**
 * A class for applying waveshaping functions to samples.
 *
 * To use this class, use the setter and getter methods to manipulate parameters,
 * and call the process method to process individual samples.
 *
 * Each sample is processed with no dependency on the previous samples, therefore a
 * single object can be reused for multiple audio streams if so desired.
 *
 * Internally relies on the parameters provided in CarveParameters.h
 */
class CarveDSPUnit {
public:
    /**
     * Sets all parameters to their default values.
     */
    CarveDSPUnit() :    preGain(PREGAIN.defaultValue),
                        postGain(POSTGAIN.defaultValue),
                        tweak(TWEAK.defaultValue),
                        mode(MODE.defaultValue) { }
    
    virtual ~CarveDSPUnit() {}
    
    /**
     * Sets the wave shape which will be applied to the signal.
     *
     * @see     ModeParameter for valid values
     *
     * @param   val Value the mode should be set to
     */
    void setMode(int val) { mode = MODE.BoundsCheck(val); }
    
    /**
     * Sets the gain to be applied to the signal before processing.
     * More pre-gain = more distortion.
     *
     * @param   val Pre-gain value that should be used
     *
     * @see     PREGAIN for valid values
     */
    void setPreGain(float val) { preGain = PREGAIN.BoundsCheck(val); }
    
    /**
     * Sets the gain to be applied to the signal after processing.
     * More post-gain = more volume.
     *     
     * @param   val Post-gain value that should be used
     *
     * @see     POSTGAIN for valid values
     */
    void setPostGain(float val) { postGain = POSTGAIN.BoundsCheck(val); }
    
    /**
     * Sets the tweak value to be applied to the signal during processing.
     * This behaves differently for each mode, and modifies the shape of
     * the wave applied to the signal
     *
     * @param   val Tweak value that should be used
     *
     * @see     TWEAK for valid values
     */
    void setTweak(float val) { tweak = TWEAK.BoundsCheck(val); }
    
    /**
     * @see     setMode
     */
    int getMode() { return mode; }
    
    /**
     * @see     setPreGain
     */
    float getPreGain() { return preGain; }
    
    /**
     * @see     setPostGain
     */
    float getPostGain() { return postGain; }
    
    /**
     * @see     setTweak
     */
    float getTweak() { return tweak; }
    
    /**
     * Performs the processing on the sample, by calling the appropriate
     * private processing methods.
     *
     * @param   inSample    The sample to be processed
     *
     * @return  The value of inSample after processing
     */
    float process (float inSample) const {
        switch (mode) {
            case MODE.SINE:
                return processSine(inSample);
                
            case MODE.PARABOLIC_SOFT:
                return processParabolicSoft(inSample);
                
            case MODE.PARABOLIC_HARD:
                return processParabolicHard(inSample);
                
            case MODE.ASYMMETRIC_SINE:
                return processAsymmetricSine(inSample);
                
            case MODE.EXPONENT:
                return processExponent(inSample);
                
            case MODE.CLIPPER:
                return processClipper(inSample);
                
            default:
                return processSine(inSample);
        }
    }
    
private:
    float   preGain,
            postGain,
            tweak;
    
    int mode;
    
    // private process methods
    inline float processSine(float inSample) const {
        return  ((((1 - fabs(tweak/2)) * sin(M_PI * inSample * preGain)))
                + ((tweak/2) * sin(4 * M_PI * inSample * preGain)))
                * postGain;
    }
    
    inline float processParabolicSoft(float inSample) const {
        return (M_PI * inSample * preGain * ((4 * tweak) - sqrt(4 * pow(inSample * M_PI * preGain, 2))) * 0.5) * postGain;
    }
    
    inline float processParabolicHard(float inSample) const {
        return  (((1 - fabs(tweak/10)) * (atan(preGain * 4 * M_PI * inSample) / 1.5))
                + ((tweak/10) * sin(M_PI * inSample * preGain)))
                * postGain;
    }
    
    inline float processAsymmetricSine(float inSample) const {
        return (cos(M_PI * inSample * (tweak + 1)) * atan(4 * M_PI * inSample * preGain)) * postGain;
    }
    
    inline float processExponent(float inSample) const {
        return  (sin(-0.25 *
                     pow(2 * M_E, (inSample * preGain + 1.5))))
                * postGain;
    }
    
    inline float processClipper(float inSample) const {
        inSample *= M_PI * preGain;
        
        return (sin(0.5 * inSample) +
                0.3 * sin(1.5 * inSample) +
                0.15 * sin(2.5 * inSample) *
                0.075 * sin(3.5 * inSample) +
                0.0375 * sin(4.5 * inSample) +
                0.01875 * sin(5.5 * inSample) +
                0.009375 * sin(6.5 * inSample)) * postGain / 1.5;
    }
};

#endif
