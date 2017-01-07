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
    }
}