/*
 *  File:       CarveDSPUnitTests.cpp
 *
 *  Version:    1.0.0
 *
 *  Created:    26/12/2016
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
#include "CoreTestLib.h"
#include "CarveDSPUnit.h"

SCENARIO("CarveDSPUnit: Parameters can be set and retrieved correctly") {
    GIVEN("A new CarveDSPUnit object") {
        CarveDSPUnit mCarve;
        
        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                REQUIRE(mCarve.getMode() == 1);
                REQUIRE(mCarve.getPreGain() == 1);
                REQUIRE(mCarve.getPostGain() == 0.5);
                REQUIRE(mCarve.getTweak() == 0);
            }
        }

        WHEN("All parameters are changed to unique values") {
            mCarve.setMode(2);
            mCarve.setPreGain(0.02);
            mCarve.setPostGain(0.03);
            mCarve.setTweak(0.04);

            THEN("They all get their correct unique values") {
                REQUIRE(mCarve.getMode() == 2);
                REQUIRE(mCarve.getPreGain() == 0.02);
                REQUIRE(mCarve.getPostGain() == 0.03);
                REQUIRE(mCarve.getTweak() == 0.04);
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Parameters enforce their bounds correctly") {
    GIVEN("A new CarveDSPUnit object") {
        CarveDSPUnit mCarve;
        
        WHEN("All parameter values are too low") {
            mCarve.setMode(-5);
            mCarve.setPreGain(-5);
            mCarve.setPostGain(-5);
            mCarve.setTweak(-5);
            
            THEN("Parameters enforce their lower bounds") {
                REQUIRE(mCarve.getMode() == 1);
                REQUIRE(mCarve.getPreGain() == 0);
                REQUIRE(mCarve.getPostGain() == 0);
                REQUIRE(mCarve.getTweak() == -1);
            }
        }
        
        WHEN("All parameter values are too high") {
            mCarve.setMode(10);
            mCarve.setPreGain(5);
            mCarve.setPostGain(5);
            mCarve.setTweak(5);
            
            THEN("Parameters enforce their upper bounds") {
                REQUIRE(mCarve.getMode() == 7);
                REQUIRE(mCarve.getPreGain() == 2);
                REQUIRE(mCarve.getPostGain() == 2);
                REQUIRE(mCarve.getTweak() == 1);
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Parameter combinations that should result in silence output for any input") {
    GIVEN("A new CarveDSPUnit object and a buffer of 0.5fs") {
        std::vector<double> buffer(1024);
        CarveDSPUnit mCarve;
        
        WHEN("The unit is turned off") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0.5);
            
            // turn the unit off
            mCarve.setMode(1);

            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }
            
            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    REQUIRE(CoreTestLib::compareFloats(buffer[iii], 0.0));
                }
            }
        }
        
        WHEN("Unit is on but has 0 pregain") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0.5);
            
            // turn the unit on, set pregain
            mCarve.setMode(2);
            mCarve.setPreGain(0);
            
            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }
            
            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    REQUIRE(CoreTestLib::compareFloats(buffer[iii], 0.0));
                }
            }
        }

        WHEN("Unit is on but has 0 postgain") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0.5);
            
            // turn the unit on, set pregain and postgain
            mCarve.setMode(2);
            mCarve.setPreGain(1);
            mCarve.setPostGain(0);
            
            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }
            
            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    REQUIRE(CoreTestLib::compareFloats(buffer[iii], 0.0));
                }
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Silence in = silence out") {
    GIVEN("A CarveDSPUnit and a buffer of silent samples") {
        std::vector<double> buffer(1024);
        CarveDSPUnit mCarve;
        
        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0);
            
            // turn the unit on
            mCarve.setMode(2);
            
            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }
            
            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    REQUIRE(CoreTestLib::compareFloats(buffer[iii], 0.0));
                }
            }
        }
    }
}
