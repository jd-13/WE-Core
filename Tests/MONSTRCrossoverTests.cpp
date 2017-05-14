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

#include "catch.hpp"
#include "MONSTRFilters/MONSTRCrossover.h"

SCENARIO("MONSTRCrossover: Parameters can be set and retrieved correctly") {
    GIVEN("A new MONSTRCrossover object") {
        MONSTRCrossover mCrossover;
        
        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mCrossover.getCrossoverLower() == Approx(100.0f));
                CHECK(mCrossover.getCrossoverUpper() == Approx(5000.0f));
                
                CHECK(mCrossover.band1.getWidth() == Approx(1.0f));
                CHECK(mCrossover.band1.getIsActive() == true);
                
                CHECK(mCrossover.band2.getWidth() == Approx(1.0f));
                CHECK(mCrossover.band2.getIsActive() == true);
                
                CHECK(mCrossover.band3.getWidth() == Approx(1.0f));
                CHECK(mCrossover.band2.getIsActive() == true);
            }
        }

        WHEN("All parameters are changed to unique values") {
            mCrossover.setCrossoverLower(41);
            mCrossover.setCrossoverUpper(3001);
            
            mCrossover.band1.setWidth(0.1f);
            mCrossover.band1.setIsActive(1);

            
            THEN("They all get their correct unique values") {
                CHECK(mCrossover.getCrossoverLower() == Approx(41.0f));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3001.0f));
                
                CHECK(mCrossover.band1.getWidth() == Approx(0.1f));
                CHECK(mCrossover.band1.getIsActive() == 1);
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Parameters enforce their bounds correctly") {
    GIVEN("A new MONSTRCrossover object") {
        MONSTRCrossover mCrossover;
        
        WHEN("All parameter values are too low") {
            mCrossover.setCrossoverLower(39);
            mCrossover.setCrossoverUpper(2999);
            mCrossover.band1.setWidth(-1);

            
            THEN("Parameters enforce their lower bounds") {
                CHECK(mCrossover.getCrossoverLower() == Approx(40.0f));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3000.0f));
                CHECK(mCrossover.band1.getWidth() == Approx(0.0f));
            }
        }

        WHEN("All parameter values are too high") {
            mCrossover.setCrossoverLower(39);
            mCrossover.setCrossoverUpper(2999);
            mCrossover.band1.setWidth(-1);
            
            THEN("Parameters enforce their upper bounds") {
                CHECK(mCrossover.getCrossoverLower() == Approx(40.0f));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3000.0f));
                CHECK(mCrossover.band1.getWidth() == Approx(0.0f));
            }
        }
    }
}
