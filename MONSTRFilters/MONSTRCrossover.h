/*
 *  File:       MONSTRCrossover.h
 *
 *  Version:    1.0.0
 *
 *  Created:    08/11/2016
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

#ifndef MONSTRCrossover_h
#define MONSTRCrossover_h

#include <vector>
#include "MONSTRBand.h"


/**
 * Class which provides three band stereo width control. 
 *
 * This class contains three MONSTRBand objects, and may later be expanded
 * to support a variable number of bands.
 *
 * To use this class, use the setter and getter methods to manipulate crossover
 * parameters which are shared by several bands (crossover frequencies), and call
 * the setter and getter methods of each band to manipulate parameters which are
 * particular to each band. Call the reset, setSampleRate and process methods as
 * required.
 *
 * @see MONSTRBand
 */
class MONSTRCrossover {
public:
    
    MONSTRBand  band1,
                band2,
                band3;
    
    /**
     * Makes each band aware of its position, and therefore which of their internal filters
     * they each need to activate.
     */
    MONSTRCrossover() : band1(true, false),
                        band2(false, false),
                        band3(false, true) {}
    
    virtual ~MONSTRCrossover() {}
    
    /**
     * Applies the filtering to a stereo buffer of samples.
     * Expect seg faults or other memory issues if arguements passed are incorrect.
     *
     * @param[out]   leftSample      Pointer to the first sample of the left channel's buffer
     * @param[out]   rightSample     Pointer to the first sample of the right channel's buffer
     * @param[in]    numSamples      Number of samples in the buffer. The left and right buffers
     *                               must be the same size.
     */
    void Process2in2out(double* leftSample, double* rightSample, size_t numSamples) {
        // make a copy of the buffers for each band to process in parallel
        std::vector<double> band1LeftBuffer(numSamples);
        std::vector<double> band1RightBuffer(numSamples);
        std::vector<double> band2LeftBuffer(numSamples);
        std::vector<double> band2RightBuffer(numSamples);
        std::vector<double> band3LeftBuffer(numSamples);
        std::vector<double> band3RightBuffer(numSamples);
        
        for (size_t iii {0}; iii < numSamples; iii++) {
            band1LeftBuffer[iii] = leftSample[iii];
            band2LeftBuffer[iii] = leftSample[iii];
            band3LeftBuffer[iii] = leftSample[iii];
            
            band1RightBuffer[iii] = rightSample[iii];
            band2RightBuffer[iii] = rightSample[iii];
            band3RightBuffer[iii] = rightSample[iii];
        }
        
        // let each band do its processing
        band1.process2in2out(band1LeftBuffer, band1RightBuffer);
        band2.process2in2out(band2LeftBuffer, band2RightBuffer);
        band3.process2in2out(band3LeftBuffer, band3RightBuffer);
        
        // combine the output from each band, and write to output
        for (size_t iii {0}; iii < numSamples; iii++) {
            leftSample[iii] = band1LeftBuffer[iii] + band2LeftBuffer[iii] + band3LeftBuffer[iii];
            rightSample[iii] = band1RightBuffer[iii] + band2RightBuffer[iii] + band3RightBuffer[iii];
        }
    }
    
    /**
     * Sets the crossover frequency of the lower (band1) and middle (band2) bands.
     *
     * @param   val   The frequency in Hz to set the lower crossover point to.
     *
     * @see     CROSSOVERLOWER for valid values
     */
    void setCrossoverLower(float val) {
        val = CROSSOVERLOWER.BoundsCheck(val);
        band1.setHighCutoff(val);
        band2.setLowCutoff(val);
    }
    
    /**
     * Sets the crossover frequency of the middle (band2) and upper (band3) bands.
     *
     * @param   val   The frequency in Hz to set the upper crossover point to.
     *
     * @see     CROSSOVERUPPER for valid values
     */
    void setCrossoverUpper(float val) {
        val = CROSSOVERUPPER.BoundsCheck(val);
        band2.setHighCutoff(val);
        band3.setLowCutoff(val);
    }
    
    /**
     * Gets the crossover frequency of the lower (band1) and middle (band2) bands.
     *
     * @return  val The frequency in Hz of the lower crossover point.
     *
     * @see     CROSSOVERLOWER for valid values
     */
    float getCrossoverLower() { return band1.getHighCutoff(); }
    
    /**
     * Gets the crossover frequency of the middle (band2) and upper (band3) bands.
     *
     * @return  val The frequency in Hz of the upper crossover point.
     *
     * @see     CROSSOVERUPPER for valid values
     */
    float getCrossoverUpper() { return band2.getHighCutoff(); }
    
    /**
     * Configures the filters for the correct sample rate. Ensure this is
     * called before attempting to process audio.
     *
     * @param   newSampleRate  The sample rate the filter should be configured for
     */
    void setSampleRate(double newSampleRate) {
        band1.setSampleRate(newSampleRate);
        band2.setSampleRate(newSampleRate);
        band3.setSampleRate(newSampleRate);
    }
    
    /**
     * Resets all filters.
     * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
     */
    void reset() {
        band1.reset();
        band2.reset();
        band3.reset();
    }
};



#endif /* MONSTRCrossover_h */
