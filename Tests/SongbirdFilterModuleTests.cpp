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
