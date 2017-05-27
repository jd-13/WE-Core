/*
 *  File:       PerformanceTests.cpp
 *
 *  Version:    1.0.0
 *
 *  Created:    26/05/2017
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

#include "catch.hpp"
#include <ctime>
#include <algorithm>
#include <iostream>
#include "CarveDSP/CarveDSPUnit.h"
#include "MONSTRFilters/MONSTRCrossover.h"

/**
 * Contains performance stats
 */
struct Stats {
    double average;
    double deviation;
};

/**
 * Contains limits for multiple performance critera
 */
struct Limits {
    const double INDIVIDUAL;
    const double AVERAGE;
    const double DEVIATION;
};

// clang complains if this isn't inlined
inline Stats calcAverageAndJitter(const std::vector<double>& executionTimes) {
    Stats retVal;
    
    // calculate the average first
    retVal.average = 0;
    for (double time : executionTimes) {
        retVal.average += time;
    }
    retVal.average = retVal.average / executionTimes.size();

    // now calculate deviation
    retVal.deviation = 0;
    for (const double& time : executionTimes) {
        retVal.deviation += std::pow((time - retVal.average), 2);
    }
    retVal.deviation = retVal.deviation / (executionTimes.size() - 1);
    retVal.deviation = std::sqrt(retVal.deviation);
    
    return retVal;
}


/*** TEST BEGIN HERE ***/

SCENARIO("Performance: CarveDSPUnit, 100 buffers of 1024 samples each") {
    GIVEN("A CarveDSPUnit and a buffer of samples") {

        const int NUM_BUFFERS {100};
        std::vector<double> buffer(1024);
        CarveDSPUnit mCarve;
        
        // set the performance limits
        Limits mLimits{0.11, 0.07, 0.005};
        
        // store the execution time for each buffer
        std::vector<double> executionTimes;

        WHEN("The samples are processed") {
            // turn the unit on
            mCarve.setMode(2);

            for (int nbuf {0}; nbuf < NUM_BUFFERS; nbuf++) {

                // fill the buffer with a sine wave
                int iii {0};
                std::generate(buffer.begin(), buffer.end(), [&iii]{ return std::sin(iii++); });

                
                // do processing
                const size_t startTime {clock()};
                for (size_t jjj {0}; jjj < buffer.size(); jjj++) {
                    buffer[jjj] = mCarve.process(buffer[jjj]);
                }
                const size_t endTime {clock()};

                // calculate the execution time
                const double CLOCKS_PER_MICROSEC {static_cast<double>(CLOCKS_PER_SEC) / 1000};
                const double executionTime {(endTime - startTime) / CLOCKS_PER_MICROSEC};
                executionTimes.push_back(executionTime);
                CHECK(executionTime < mLimits.INDIVIDUAL);
            }
            
            THEN("The average and variance are within limits") {
                Stats mStats = calcAverageAndJitter(executionTimes);
                CHECK(mStats.average < mLimits.AVERAGE);
                CHECK(mStats.deviation < mLimits.DEVIATION);
            }
        }
    }
}

SCENARIO("Performance: MONSTRCrossover, 100 buffers of 1024 samples each") {
    GIVEN("A MONSTRCrossover and a buffer of  samples") {
        
        const int NUM_BUFFERS {100};
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        MONSTRCrossover mCrossover;
        
        // set the performance limits
        Limits mLimits{1.0, 0.8, 0.08};
        
        // store the execution time for each buffer
        std::vector<double> executionTimes;
        
        WHEN("The silence samples are processed") {
            // turn the crossover on
            mCrossover.band1.setIsActive(true);
            mCrossover.band2.setIsActive(true);
            mCrossover.band3.setIsActive(true);

            for (int nbuf {0}; nbuf < NUM_BUFFERS; nbuf++) {

                // fill the buffer with a sine wave
                int iii {0};
                std::generate(leftBuffer.begin(), leftBuffer.end(), [&iii]{ return std::sin(iii++); });
                rightBuffer = leftBuffer;
                
                
                // do processing
                const size_t startTime {clock()};
                mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());
                const size_t endTime {clock()};

                // calculate the execution time
                const double CLOCKS_PER_MICROSEC {static_cast<double>(CLOCKS_PER_SEC) / 1000};
                const double executionTime {(endTime - startTime) / CLOCKS_PER_MICROSEC};
                executionTimes.push_back(executionTime);
                CHECK(executionTime < mLimits.INDIVIDUAL);
            }
            
            THEN("The average and variance are within limits") {
                Stats mStats = calcAverageAndJitter(executionTimes);
                CHECK(mStats.average < mLimits.AVERAGE);
                CHECK(mStats.deviation < mLimits.DEVIATION);
            }
        }
    }
}
