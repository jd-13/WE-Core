DSPFILTERS_PATH = DSPFilters/shared/DSPFilters

GCOVFLAGS = -fprofile-arcs -ftest-coverage
CXXFLAGS_COMMON = -std=c++11 -Wall -Werror -Wextra -Wconversion -Wshadow $(GCOVFLAGS)

# Set the GCC and Clang flags
# TODO: work on removing the need for the below Wno flags
CXXFLAGS_CLANG = $(CXXFLAGS_COMMON) -Weverything -Wpedantic -Wno-exit-time-destructors -Wno-weak-vtables -Wno-reserved-id-macro -Wno-double-promotion -Wno-unknown-warning-option
CXXFLAGS_GCC = $(CXXFLAGS_COMMON) -pedantic

# Set # Set the GCC and Clang flags for the DSPFilters library
CXXFLAGS_CLANG_DSP = $(CXXFLAGS_CLANG) -Wno-float-equal -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-old-style-cast -Wno-padded -Wno-unused-variable -Wno-sign-conversion
CXXFLAGS_GCC_DSP = $(CXXFLAGS_COMMON) -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-variable -Wno-sign-conversion -Wno-unknown-warning-option

# Set the flags we'll actually use based on the compiler
CXXFLAGS = $(CXXFLAGS_GCC)
CXXFLAGS_DSP = $(CXXFLAGS_GCC_DSP)
ifeq ($(CXX), $(filter $(CXX),clang++ clang++-3.7))
CXXFLAGS = $(CXXFLAGS_CLANG)
CXXFLAGS_DSP = $(CXXFLAGS_CLANG_DSP)
endif

OBJDIR = obj

# Build rules start here
default: WECoreTest


# Build test objects
TEST_OBJS = $(addprefix $(OBJDIR)/, catchMain.o CarveDSPUnitTests.o RichterLFOPairTests.o SongbirdFilterModuleTests.o TPTSVFilterTests.o MONSTRCrossoverTests.o)

$(OBJDIR)/%.o: $(WECORE_SRC)/Tests/%.cpp
	$(CXX) -c $< -o $@ -I$(CATCH_PATH) -I$(WECORE_SRC) -I$(DSPFILTERS_PATH)/include/ $(CXXFLAGS)


# Build DSP Filters objects
DSP_OBJS = $(addprefix $(OBJDIR)/, Biquad.o PoleFilter.o Butterworth.o Cascade.o)

$(OBJDIR)/%.o: $(DSPFILTERS_PATH)/source/%.cpp
	$(CXX) -c $< -o $@ -I$(DSPFILTERS_PATH)/include/ $(CXXFLAGS_DSP)

createDir:
	mkdir -p $(OBJDIR)

WECoreTest: createDir $(DSP_OBJS) $(TEST_OBJS)
	mkdir -p $(OBJDIR)
	$(CXX) $(DSP_OBJS) $(TEST_OBJS) -o WECoreTest $(GCOVFLAGS)

clean:
	rm -r $(OBJDIR)
	rm WECoreTest
