/*
 *  File:       RichterLFOPairTests.cpp
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
#include "RichterLFOPair.h"

SCENARIO("RichterLFOPair: Parameters can be set and retrieved correctly") {
    GIVEN("A new RichterLFOPair object") {
        RichterLFOPair mLFOPair;
        
        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                REQUIRE(mLFOPair.LFO.getBypassSwitch() == false);
                REQUIRE(mLFOPair.LFO.getPhaseSyncSwitch() == false);
                REQUIRE(mLFOPair.LFO.getTempoSyncSwitch() == false);
                REQUIRE(mLFOPair.LFO.getWave() == 1);
                REQUIRE(mLFOPair.LFO.getDepth() == 0.5);
                REQUIRE(mLFOPair.LFO.getDepthMod() == 0);
                REQUIRE(mLFOPair.LFO.getFreq() == 2);
                REQUIRE(mLFOPair.LFO.getFreqMod() == 0);
                REQUIRE(mLFOPair.LFO.getManualPhase() == 0);
                REQUIRE(mLFOPair.LFO.getTempoNumer() == 1);
                REQUIRE(mLFOPair.LFO.getTempoDenom() == 1);
                
                REQUIRE(mLFOPair.MOD.getBypassSwitch() == false);
                REQUIRE(mLFOPair.MOD.getPhaseSyncSwitch() == false);
                REQUIRE(mLFOPair.MOD.getTempoSyncSwitch() == false);
                REQUIRE(mLFOPair.MOD.getWave() == 1);
                REQUIRE(mLFOPair.MOD.getDepth() == 0.5);
                REQUIRE(mLFOPair.MOD.getFreq() == 2);
                REQUIRE(mLFOPair.MOD.getManualPhase() == 0);
                REQUIRE(mLFOPair.MOD.getTempoNumer() == 1);
                REQUIRE(mLFOPair.MOD.getTempoDenom() == 1);
            }
        }
        
        WHEN("All parameters are changed to unique values") {
            mLFOPair.LFO.setBypassSwitch(true);
            mLFOPair.LFO.setPhaseSyncSwitch(true);
            mLFOPair.LFO.setTempoSyncSwitch(true);
            mLFOPair.LFO.setWave(2);
            mLFOPair.LFO.setDepth(0.1);
            mLFOPair.LFO.setDepthMod(0.2);
            mLFOPair.LFO.setFreq(3);
            mLFOPair.LFO.setFreqMod(0.4);
            mLFOPair.LFO.setManualPhase(5);
            mLFOPair.LFO.setTempoNumer(2);
            mLFOPair.LFO.setTempoDenom(3);
            
            mLFOPair.MOD.setBypassSwitch(true);
            mLFOPair.MOD.setPhaseSyncSwitch(true);
            mLFOPair.MOD.setTempoSyncSwitch(true);
            mLFOPair.MOD.setWave(3);
            mLFOPair.MOD.setDepth(0.5);
            mLFOPair.MOD.setFreq(6);
            mLFOPair.MOD.setManualPhase(7);
            mLFOPair.MOD.setTempoNumer(3);
            mLFOPair.MOD.setTempoDenom(4);
            
            THEN("They all get their correct unique values") {
                REQUIRE(mLFOPair.LFO.getBypassSwitch() == true);
                REQUIRE(mLFOPair.LFO.getPhaseSyncSwitch() == true);
                REQUIRE(mLFOPair.LFO.getTempoSyncSwitch() == true);
                REQUIRE(mLFOPair.LFO.getWave() == 2);
                REQUIRE(mLFOPair.LFO.getDepth() == 0.1);
                REQUIRE(mLFOPair.LFO.getDepthMod() == 0.2);
                REQUIRE(mLFOPair.LFO.getFreq() == 3);
                REQUIRE(mLFOPair.LFO.getFreqMod() == 0.4);
                REQUIRE(mLFOPair.LFO.getManualPhase() == 5);
                REQUIRE(mLFOPair.LFO.getTempoNumer() == 2);
                REQUIRE(mLFOPair.LFO.getTempoDenom() == 3);
                
                REQUIRE(mLFOPair.MOD.getBypassSwitch() == true);
                REQUIRE(mLFOPair.MOD.getPhaseSyncSwitch() == true);
                REQUIRE(mLFOPair.MOD.getTempoSyncSwitch() == true);
                REQUIRE(mLFOPair.MOD.getWave() == 3);
                REQUIRE(mLFOPair.MOD.getDepth() == 0.5);
                REQUIRE(mLFOPair.MOD.getFreq() == 6);
                REQUIRE(mLFOPair.MOD.getManualPhase() == 7);
                REQUIRE(mLFOPair.MOD.getTempoNumer() == 3);
                REQUIRE(mLFOPair.MOD.getTempoDenom() == 4);
            }
        }
    }
}

SCENARIO("RichterLFOPair: Parameters enforce their bounds correctly") {
    GIVEN("A new CarveDSPUnit object") {
        RichterLFOPair mLFOPair;
        
        WHEN("All parameter values are too low") {
            mLFOPair.LFO.setWave(-5);
            mLFOPair.LFO.setDepth(-5);
            mLFOPair.LFO.setDepthMod(-5);
            mLFOPair.LFO.setFreq(-5);
            mLFOPair.LFO.setFreqMod(-5);
            mLFOPair.LFO.setManualPhase(-5);
            mLFOPair.LFO.setTempoNumer(-5);
            mLFOPair.LFO.setTempoDenom(-5);
            
            mLFOPair.MOD.setWave(-5);
            mLFOPair.MOD.setDepth(-5);
            mLFOPair.MOD.setFreq(-5);
            mLFOPair.MOD.setManualPhase(-5);
            mLFOPair.MOD.setTempoNumer(-5);
            mLFOPair.MOD.setTempoDenom(-5);
            
            THEN("Parameters enforce their lower bounds") {
                REQUIRE(mLFOPair.LFO.getWave() == 1);
                REQUIRE(mLFOPair.LFO.getDepth() == 0);
                REQUIRE(mLFOPair.LFO.getDepthMod() == 0);
                REQUIRE(mLFOPair.LFO.getFreq() == 0.5);
                REQUIRE(mLFOPair.LFO.getFreqMod() == 0);
                REQUIRE(mLFOPair.LFO.getManualPhase() == 0);
                REQUIRE(mLFOPair.LFO.getTempoNumer() == 1);
                REQUIRE(mLFOPair.LFO.getTempoDenom() == 1);

                REQUIRE(mLFOPair.MOD.getWave() == 1);
                REQUIRE(mLFOPair.MOD.getDepth() == 0);
                REQUIRE(mLFOPair.MOD.getFreq() == 0.5);
                REQUIRE(mLFOPair.MOD.getManualPhase() == 0);
                REQUIRE(mLFOPair.MOD.getTempoNumer() == 1);
                REQUIRE(mLFOPair.MOD.getTempoDenom() == 1);
            }
        }
        
        WHEN("All parameter values are too high") {
            mLFOPair.LFO.setWave(100);
            mLFOPair.LFO.setDepth(100);
            mLFOPair.LFO.setDepthMod(100);
            mLFOPair.LFO.setFreq(100);
            mLFOPair.LFO.setFreqMod(100);
            mLFOPair.LFO.setManualPhase(10000);
            mLFOPair.LFO.setTempoNumer(100);
            mLFOPair.LFO.setTempoDenom(100);
            
            mLFOPair.MOD.setWave(100);
            mLFOPair.MOD.setDepth(100);
            mLFOPair.MOD.setFreq(100);
            mLFOPair.MOD.setManualPhase(10000);
            mLFOPair.MOD.setTempoNumer(100);
            mLFOPair.MOD.setTempoDenom(100);
            
            THEN("Parameters enforce their upper bounds") {
                REQUIRE(mLFOPair.LFO.getWave() == 3);
                REQUIRE(mLFOPair.LFO.getDepth() == 1);
                REQUIRE(mLFOPair.LFO.getDepthMod() == 1);
                REQUIRE(mLFOPair.LFO.getFreq() == 20);
                REQUIRE(mLFOPair.LFO.getFreqMod() == 1);
                REQUIRE(mLFOPair.LFO.getManualPhase() == 2000);
                REQUIRE(mLFOPair.LFO.getTempoNumer() == 4);
                REQUIRE(mLFOPair.LFO.getTempoDenom() == 32);
                
                REQUIRE(mLFOPair.MOD.getWave() == 3);
                REQUIRE(mLFOPair.MOD.getDepth() == 1);
                REQUIRE(mLFOPair.MOD.getFreq() == 20);
                REQUIRE(mLFOPair.MOD.getManualPhase() == 2000);
                REQUIRE(mLFOPair.MOD.getTempoNumer() == 4);
                REQUIRE(mLFOPair.MOD.getTempoDenom() == 32);
            }
        }
    }
}
