/*
 *  File:       MONSTRCrossoverTests.cpp
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
#include "MONSTRFilters/MONSTRBand.h"
#include "WEFilters/Tests/TestUtils.h"

SCENARIO("MONSTRBand: Parameters can be set and retrieved correctly") {
    GIVEN("A new MONSTRBand object") {
        WECore::MONSTR::MONSTRBand<double> mBand(WECore::MONSTR::BandType::MIDDLE);

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mBand.getIsActive() == true);
                CHECK(mBand.getIsMuted() == false);
                CHECK(mBand.getLowCutoff() == Approx(100.0));
                CHECK(mBand.getHighCutoff() == Approx(5000.0));
            }
        }

        WHEN("All parameters are changed to unique values") {

            mBand.setIsActive(false);
            mBand.setIsMuted(false);
            mBand.setLowCutoff(201);
            mBand.setHighCutoff(6001);

            THEN("They all get their correct unique values") {
                CHECK(mBand.getIsActive() == false);
                CHECK(mBand.getIsMuted() == false);
                CHECK(mBand.getLowCutoff() == Approx(201.0));
                CHECK(mBand.getHighCutoff() == Approx(6001.0));
            }
        }
    }
}

SCENARIO("MONSTRBand: Parameters enforce their bounds correctly") {
    GIVEN("A new MONSTRBand object") {
        WECore::MONSTR::MONSTRBand<double> mBand(WECore::MONSTR::BandType::MIDDLE);

        WHEN("All parameter values are too low") {
            mBand.setLowCutoff(39);
            mBand.setHighCutoff(39);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mBand.getLowCutoff() == Approx(40.0));
                CHECK(mBand.getHighCutoff() == Approx(40.0));
            }
        }

        WHEN("All parameter values are too high") {
            mBand.setLowCutoff(20000);
            mBand.setHighCutoff(20000);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mBand.getLowCutoff() == Approx(19500.0));
                CHECK(mBand.getHighCutoff() == Approx(19500.0));
            }
        }

        WHEN("The high cutoff is set below the lower one") {
            mBand.setLowCutoff(4000);
            mBand.setHighCutoff(2000);

            THEN("Both frequencies move to the lower value") {
                CHECK(mBand.getLowCutoff() == Approx(2000.0));
                CHECK(mBand.getHighCutoff() == Approx(2000.0));
            }
        }

        WHEN("The lower cutoff is set above the higher one") {
            mBand.setHighCutoff(2000);
            mBand.setLowCutoff(4000);

            THEN("Both frequencies move to the higher value") {
                CHECK(mBand.getLowCutoff() == Approx(4000.0));
                CHECK(mBand.getHighCutoff() == Approx(4000.0));
            }
        }
    }
}


SCENARIO("MONSTRBand: Muted") {
    GIVEN("A MONSTRBand and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);

        WECore::MONSTR::MONSTRBand<double> mBand(WECore::MONSTR::BandType::MIDDLE);

        WHEN("The band is muted") {

            mBand.setIsMuted(true);

            // Fill the buffer
            WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            // Do processing
            mBand.process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is silence") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(0.0));
                    CHECK(rightBuffer[index] == Approx(0.0));
                }
            }
        }
    }
}
