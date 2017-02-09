/*
 *  File:       SongbirdFilterModuleTests.cpp
 *
 *  Version:    1.0.0
 *
 *  Created:    07/01/2017
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
#include "SongbirdFilterModule.h"

SCENARIO("SongbirdFilterModule: Parameters can be set and retrieved correctly") {
    GIVEN("A new SongbirdFilterModule object") {
        SongbirdFilterModule mSongbird;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                REQUIRE(mSongbird.getVowel1() == 1);
                REQUIRE(mSongbird.getVowel2() == 2);
                REQUIRE(mSongbird.getFilterPosition() == 0.5);
                REQUIRE(mSongbird.getMix() == 1.0);
                REQUIRE(mSongbird.getModMode() == false);
            }
        }

        WHEN("All parameters are changed to unique values") {
            mSongbird.setVowel1(3);
            mSongbird.setVowel2(4);
            mSongbird.setFilterPosition(0.03);
            mSongbird.setMix(0.04);
            mSongbird.setModMode(true);

            THEN("They all get their correct unique values") {
                REQUIRE(mSongbird.getVowel1() == 3);
                REQUIRE(mSongbird.getVowel2() == 4);
                REQUIRE(mSongbird.getFilterPosition() == 0.03);
                REQUIRE(mSongbird.getMix() == 0.04);
                REQUIRE(mSongbird.getModMode() == true);
            }
        }
    }
}

SCENARIO("SongbirdFilterModule: Parameters enforce their bounds correctly") {
    GIVEN("A new SongbirdFilterModule object") {
        SongbirdFilterModule mSongbird;
        
        WHEN("All parameter values are too low") {
            mSongbird.setVowel1(-5);
            mSongbird.setVowel2(-5);
            mSongbird.setFilterPosition(-5);
            mSongbird.setMix(-5);
            
            THEN("Parameters enforce their lower bounds") {
                REQUIRE(mSongbird.getVowel1() == 1);
                REQUIRE(mSongbird.getVowel2() == 1);
                REQUIRE(mSongbird.getFilterPosition() == 0);
                REQUIRE(mSongbird.getMix() == 0);
            }
        }
        
        WHEN("All parameter values are too high") {
            mSongbird.setVowel1(1000);
            mSongbird.setVowel2(1000);
            mSongbird.setFilterPosition(1000);
            mSongbird.setMix(1000);
            
            THEN("Parameters enforce their upper bounds") {
                REQUIRE(mSongbird.getVowel1() == 5);
                REQUIRE(mSongbird.getVowel2() == 5);
                REQUIRE(mSongbird.getFilterPosition() == 1);
                REQUIRE(mSongbird.getMix() == 1);
            }
        }
    }
}

SCENARIO("SongbirdFilterModule: Silence in = silence out") {
    GIVEN("A SongbirdFilterModule and a buffer of silent samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        SongbirdFilterModule mSongbird;
        
        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(leftBuffer.begin(), leftBuffer.end(), 0);
            std::fill(rightBuffer.begin(), rightBuffer.end(), 0);
            
            // do processing
            mSongbird.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());
            
            THEN("The output is silence") {
                for (size_t iii {0}; iii < leftBuffer.size(); iii++) {
                    REQUIRE(CoreTestLib::compareFloats(leftBuffer[iii], 0.0));
                    REQUIRE(CoreTestLib::compareFloats(rightBuffer[iii], 0.0));
                }
            }
        }
    }
}
