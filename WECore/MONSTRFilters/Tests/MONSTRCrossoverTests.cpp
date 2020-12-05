/*
 *  File:       MONSTRCrossoverTests.cpp
 *
 *  Version:    1.0.0
 *
 *  Created:    12/05/2017
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

#include <memory>

#include "catch.hpp"
#include "TestData.h"
#include "General/CoreMath.h"
#include "MONSTRFilters/MONSTRCrossover.h"
#include "WEFilters/Tests/TestUtils.h"
#include "WEFilters/StereoWidthProcessor.h"

SCENARIO("MONSTRCrossover: Parameters can be set and retrieved correctly") {
    GIVEN("A new MONSTRCrossover object") {
        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mCrossover.getCrossoverLower() == Approx(100.0));
                CHECK(mCrossover.getCrossoverUpper() == Approx(5000.0));

                CHECK(mCrossover.band1.getIsActive() == true);
                CHECK(mCrossover.band2.getIsActive() == true);
                CHECK(mCrossover.band2.getIsActive() == true);
            }
        }

        WHEN("All parameters are changed to unique values") {
            mCrossover.setCrossoverLower(41);
            mCrossover.setCrossoverUpper(3001);

            mCrossover.band1.setIsActive(1);

            THEN("They all get their correct unique values") {
                CHECK(mCrossover.getCrossoverLower() == Approx(41.0));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3001.0));

                CHECK(mCrossover.band1.getIsActive() == 1);
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Parameters enforce their bounds correctly") {
    GIVEN("A new MONSTRCrossover object") {
        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("All parameter values are too low") {
            mCrossover.setCrossoverLower(39);
            mCrossover.setCrossoverUpper(2999);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mCrossover.getCrossoverLower() == Approx(40.0));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3000.0));
            }
        }

        WHEN("All parameter values are too high") {
            mCrossover.setCrossoverLower(39);
            mCrossover.setCrossoverUpper(2999);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mCrossover.getCrossoverLower() == Approx(40.0));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3000.0));
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Silence in = silence out") {
    GIVEN("A MONSTRCrossover, processors, and a buffer of silent samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.band1.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band2.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band3.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(leftBuffer.begin(), leftBuffer.end(), 0);
            std::fill(rightBuffer.begin(), rightBuffer.end(), 0);

            // do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is silence") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(0.0).margin(0.00001));
                    CHECK(rightBuffer[index] == Approx(0.0).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Sine in = sine out") {
    GIVEN("A MONSTRCrossover, processors, and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::MONSTR::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.band1.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band2.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band3.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

        WHEN("The bands are all active with neutral processing") {
            // Fill the buffer
            WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            // Do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The expected output is produced") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(expectedOutput[index]).margin(0.00001));
                    CHECK(rightBuffer[index] == Approx(expectedOutput[index]).margin(0.00001));
                }
            }
        }

        WHEN("The bands are all bypassed") {
            // Bypass the bands
            mCrossover.band1.setIsActive(0);
            mCrossover.band2.setIsActive(0);
            mCrossover.band3.setIsActive(0);

            // Fill the buffer
            WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            // Do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The expected output is produced") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(expectedOutput[index]).margin(0.00001));
                    CHECK(rightBuffer[index] == Approx(expectedOutput[index]).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Small buffer") {
    GIVEN("A MONSTRCrossover and a buffer of sine samples smaller than the internal buffer") {
        std::vector<double> leftBuffer(100);
        std::vector<double> rightBuffer(100);
        const std::vector<double>& expectedOutput =
                TestData::MONSTR::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.band1.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band2.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band3.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

        // Fill the buffers
        WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
        std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

        WHEN("The bands are all active") {

            // Do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The expected output is produced") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(expectedOutput[index]).margin(0.00001));
                    CHECK(rightBuffer[index] == Approx(expectedOutput[index]).margin(0.00001));
                }
            }
        }
    }
}
