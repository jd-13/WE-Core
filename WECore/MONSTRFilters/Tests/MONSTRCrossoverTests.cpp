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
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(100.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(5000.0));

                CHECK(mCrossover.getIsActive(0) == true);
                CHECK(mCrossover.getIsActive(1) == true);
                CHECK(mCrossover.getIsActive(2) == true);

                CHECK(mCrossover.getIsMuted(0) == false);
                CHECK(mCrossover.getIsMuted(1) == false);
                CHECK(mCrossover.getIsMuted(2) == false);

                CHECK(mCrossover.getIsSoloed(0) == false);
                CHECK(mCrossover.getIsSoloed(1) == false);
                CHECK(mCrossover.getIsSoloed(2) == false);
            }
        }

        WHEN("All parameters are changed to unique values") {
            mCrossover.setCrossoverFrequency(0, 41);
            mCrossover.setCrossoverFrequency(1, 3001);

            mCrossover.setIsActive(0, false);
            mCrossover.setIsMuted(0, true);
            mCrossover.setIsSoloed(1, true);

            THEN("They all get their correct unique values") {
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(41.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(3001.0));

                CHECK(mCrossover.getIsActive(0) == false);
                CHECK(mCrossover.getIsMuted(0) == true);
                CHECK(mCrossover.getIsSoloed(1) == true);
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Parameters enforce their bounds correctly") {
    GIVEN("A new MONSTRCrossover object") {
        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("All parameter values are too low") {
            mCrossover.setCrossoverFrequency(0, 39);
            mCrossover.setCrossoverFrequency(1, 39);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(40.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(40.0));
            }
        }

        WHEN("All parameter values are too high") {
            mCrossover.setCrossoverFrequency(0, 20000);
            mCrossover.setCrossoverFrequency(1, 20000);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(19500.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(19500.0));
            }
        }

        WHEN("A higher crossover frequency is set below a lower one") {
            mCrossover.setCrossoverFrequency(0, 4000);
            mCrossover.setCrossoverFrequency(1, 2000);

            THEN("Both frequencies move to the lower value") {
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(2000.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(2000.0));
            }
        }

        WHEN("A lower crossover frequency is set above a higher one") {
            mCrossover.setCrossoverFrequency(1, 2000);
            mCrossover.setCrossoverFrequency(0, 4000);

            THEN("Both frequencies move to the higher value") {
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(4000.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(4000.0));
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Silence in = silence out") {
    GIVEN("A MONSTRCrossover, processors, and a buffer of silent samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.setEffectsProcessor(0, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.setEffectsProcessor(1, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.setEffectsProcessor(2, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

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
        mCrossover.setEffectsProcessor(0, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.setEffectsProcessor(1, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.setEffectsProcessor(2, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

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
            mCrossover.setIsActive(0, false);
            mCrossover.setIsActive(1, false);
            mCrossover.setIsActive(2, false);

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
        mCrossover.setEffectsProcessor(0, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.setEffectsProcessor(1, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.setEffectsProcessor(2, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

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

SCENARIO("MONSTRCrossover: Adding a band - existing crossover frequency below maximum") {
    GIVEN("A MONSTRCrossover with 3 bands and the highest crossover frequency below the maximum") {

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("A new band is added") {

            mCrossover.addBand();

            THEN("The new crossover freq is halfway between the old one and the maximum") {
                CHECK(mCrossover.getNumBands() == 4);
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(100.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(5000.0));
                CHECK(mCrossover.getCrossoverFrequency(2) == Approx(12250.0));
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Adding a band - existing crossover at maximum") {
    GIVEN("A MONSTRCrossover with 3 bands and the highest crossover frequency at the maximum") {

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.setCrossoverFrequency(1, WECore::MONSTR::Parameters::CROSSOVER_FREQUENCY.maxValue);

        WHEN("A new band is added") {

            mCrossover.addBand();

            THEN("The old highest crossover frequency is moved down to halfway between the one below and the maximum") {
                CHECK(mCrossover.getNumBands() == 4);
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(100.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(9800.0));
                CHECK(mCrossover.getCrossoverFrequency(2) == Approx(19500.0));
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Adding a band - existing crossover at maximum and only 2 bands") {
    GIVEN("A MONSTRCrossover with 2 bands and the crossover at the maximum") {

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.removeBand();
        mCrossover.setCrossoverFrequency(0, 19500);

        WHEN("A new band is added") {

            mCrossover.addBand();

            THEN("The old highest crossover is moved down half of the maximum, the new one is at the maximum") {
                CHECK(mCrossover.getNumBands() == 3);
                CHECK(mCrossover.getCrossoverFrequency(0) == Approx(9750.0));
                CHECK(mCrossover.getCrossoverFrequency(1) == Approx(19500.0));
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Adding a band - at maximum number of bands") {
    GIVEN("A MONSTRCrossover with 6 bands") {

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.addBand();
        mCrossover.addBand();
        mCrossover.addBand();

        WHEN("A new band is added") {

            mCrossover.addBand();

            THEN("The band isn't actually added") {
                CHECK(mCrossover.getNumBands() == 6);
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Removing bands") {
    GIVEN("A MONSTRCrossover with 3 bands") {

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("A band is removed") {

            mCrossover.removeBand();

            THEN("The band is actually removed") {
                CHECK(mCrossover.getNumBands() == 2);
            }
        }

        WHEN("Another band is removed") {

            mCrossover.removeBand();

            THEN("The band isn't removed as it is already at the minimum number") {
                CHECK(mCrossover.getNumBands() == 2);
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Added band has clean configuration") {
    GIVEN("A MONSTRCrossover with 4 bands, and non-default parameters on the 4th band") {

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.addBand();
        mCrossover.setIsActive(3, false);
        mCrossover.setIsMuted(3, true);
        mCrossover.setCrossoverFrequency(2, 15000);

        auto processorStrong = std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>();
        mCrossover.setEffectsProcessor(3, processorStrong);

        // Take a weak pointer to the effects processor and reset the original shared (strong)
        // pointer - this will be expired if the shared pointer held by the band is correctly reset
        std::weak_ptr<WECore::StereoWidth::StereoWidthProcessor<double>> processorWeak = processorStrong;
        processorStrong.reset();

        WHEN("The 4th band is removed and added again") {
            mCrossover.removeBand();
            mCrossover.addBand();

            THEN("The band has the default parameters") {
                CHECK(mCrossover.getIsActive(3) == true);
                CHECK(mCrossover.getIsMuted(3) == false);
                CHECK(mCrossover.getCrossoverFrequency(2) == Approx(12250));
                CHECK(processorWeak.expired() == true);
            }
        }
    }
}

SCENARIO("MONSTRCrossover: All bands muted") {
    GIVEN("A MONSTRCrossover and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("All bands are muted and the samples processeed") {
            mCrossover.setIsMuted(0, true);
            mCrossover.setIsMuted(1, true);
            mCrossover.setIsMuted(2, true);

            // Fill the buffer
            WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            // Do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is silence") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(0.0));
                    CHECK(rightBuffer[index] == Approx(0.0));
                }
            }
        }

        WHEN("All bands are muted, effects processors are provided, and the samples processed") {

            mCrossover.setIsMuted(0, true);
            mCrossover.setIsMuted(1, true);
            mCrossover.setIsMuted(2, true);

            mCrossover.setEffectsProcessor(0, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
            mCrossover.setEffectsProcessor(1, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
            mCrossover.setEffectsProcessor(2, std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

            // Fill the buffer
            WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            // Do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is still silence") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(0.0));
                    CHECK(rightBuffer[index] == Approx(0.0));
                }
            }
        }
    }
}

SCENARIO("MONSTRCrossover: A band is soloed and muted") {
    GIVEN("A MONSTRCrossover and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("A band is soloed and muted, and the samples processeed") {
            mCrossover.setIsMuted(0, true);
            mCrossover.setIsSoloed(0, true);

            // Fill the buffer
            WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            // Do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is silence") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(0.0));
                    CHECK(rightBuffer[index] == Approx(0.0));
                }
            }
        }
    }
}

SCENARIO("MONSTRCrossover: All bands are soloed") {
    GIVEN("A MONSTRCrossover and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::MONSTR::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("All bands are soloed, and the samples processeed") {
            mCrossover.setIsSoloed(0, true);
            mCrossover.setIsSoloed(1, true);
            mCrossover.setIsSoloed(2, true);

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

SCENARIO("MONSTRCrossover: EffectsProcessors are not called for muted bands") {
    GIVEN("A MONSTRCrossover and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        // Create a processor that will fail the test if called
        class TestProcessor : public WECore::EffectsProcessor2in2out<double> {
        public:
            void process2in2out(double* /*inSamplesLeft*/,
                                double* /*inSamplesRight*/,
                                size_t /*numSamples*/) {
                FAIL_CHECK("Processor should not have been called");
            }
        };

        WHEN("A processor is assigned to a band which is muted") {
            mCrossover.setIsMuted(0, true);
            mCrossover.setEffectsProcessor(0, std::make_shared<TestProcessor>());

            // Fill the buffer
            WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            THEN("The processor is not called during processing") {
                // Do processing
                mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());
            }
        }
    }
}

SCENARIO("MONSTRCrossover: EffectsProcessors are not called for bands that aren't soloed") {
    GIVEN("A MONSTRCrossover and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        // Create a processor that will fail the test if called
        class TestProcessor : public WECore::EffectsProcessor2in2out<double> {
        public:
            void process2in2out(double* /*inSamplesLeft*/,
                                double* /*inSamplesRight*/,
                                size_t /*numSamples*/) {
                FAIL_CHECK("Processor should not have been called");
            }
        };

        WHEN("A processor is assigned to a band but another is soloed") {
            mCrossover.setIsSoloed(1, true);
            mCrossover.setEffectsProcessor(0, std::make_shared<TestProcessor>());
            mCrossover.setEffectsProcessor(2, std::make_shared<TestProcessor>());

            // Fill the buffer
            WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            THEN("The processor is not called during processing") {
                // Do processing
                mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());
            }
        }
    }
}
