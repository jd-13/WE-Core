GCOVFLAGS = -fprofile-arcs -ftest-coverage
CXXFLAGS = -std=c++11 -Wall -Werror -Wextra -Wconversion -Wshadow $(GCOVFLAGS)

DSPFILTERS_PATH = DSPFilters/shared/DSPFilters

ifeq ($(CXX), clang++)
CXXFLAGS += -Weverything -Wpedantic

# TODO: work on removing the need for the below flags
CXXFLAGS += -Wno-exit-time-destructors -Wno-weak-vtables -Wno-reserved-id-macro -Wno-double-promotion

else
CXXFLAGS += -pedantic
endif

default: WECoreTest

# Build test objects
TEST_OBJS = catchMain.o CarveDSPUnitTests.o RichterLFOPairTests.o SongbirdFilterModuleTests.o TPTSVFilterTests.o MONSTRCrossoverTests.o

%.o: $(WECORE_SRC)/Tests/%.cpp
	$(CXX) -c $< -o $@ -I$(CATCH_PATH) -I$(WECORE_SRC) -I$(DSPFILTERS_PATH)/include/ $(CXXFLAGS)


# Build DSP Filters objects
DSP_FLAGS = -Wno-float-equal -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-old-style-cast -Wno-padded -Wno-unused-variable -Wno-sign-conversion
DSP_OBJS = Biquad.o PoleFilter.o Butterworth.o Cascade.o

%.o: $(DSPFILTERS_PATH)/source/%.cpp
	$(CXX) -c $< -o $@ -I$(DSPFILTERS_PATH)/include/ $(CXXFLAGS) $(DSP_FLAGS)


WECoreTest: $(DSP_OBJS) $(TEST_OBJS)
	$(CXX) $(DSP_OBJS) $(TEST_OBJS) -o WECoreTest $(GCOVFLAGS)

clean:
	rm *.o
	rm *.gc*
	rm WECoreTest
