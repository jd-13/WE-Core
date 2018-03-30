cmake_minimum_required(VERSION 3.0)
project(WECore)

set(DSPFILTERS_PATH $ENV{WECORE_SRC}/DSPFilters/shared/DSPFilters)

include_directories($ENV{CATCH_PATH}
                    $ENV{WECORE_SRC}
                    ${DSPFILTERS_PATH}/include)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GCOVFLAGS "-fprofile-arcs -ftest-coverage")
set(CXXFLAGS_COMMON "-std=c++14 -g -Wall -Werror -Wextra -Wconversion -Wshadow ${GCOVFLAGS}")

# Set the GCC and Clang flags
# TODO: work on removing the need for the below Wno flags
set(CXXFLAGS_CLANG "${CXXFLAGS_COMMON} -Weverything -Wpedantic -Wno-exit-time-destructors -Wno-weak-vtables -Wno-reserved-id-macro -Wno-double-promotion -Wno-unknown-warning-option -Wno-c++98-compat -Wno-padded")
set(CXXFLAGS_GCC "${CXXFLAGS_COMMON} -pedantic")

# Set the GCC and Clang flags for the DSPFilters library
set(CXXFLAGS_CLANG_DSP "${CXXFLAGS_CLANG} -Wno-float-equal -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-old-style-cast -Wno-padded -Wno-unused-variable -Wno-sign-conversion -Wno-missing-noreturn -Wno-zero-as-null-pointer-constant")
set(CXXFLAGS_GCC_DSP "${CXXFLAGS_COMMON} -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-variable -Wno-sign-conversion -Wno-missing-noreturn")

# Set the flags we'll actually use based on the compiler
set(CXXFLAGS ${CXXFLAGS_CLANG})
set(CXXFLAGS_DSP ${CXXFLAGS_CLANG_DSP})

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CXXFLAGS ${CXXFLAGS_GCC})
    set(CXXFLAGS_DSP ${CXXFLAGS_GCC_DSP})
endif()

# TODO: For now we have to use the DSP flags, as everything will be able to build using them
set(CMAKE_CXX_FLAGS ${CXXFLAGS_DSP})

# Set the source files
set(TEST_PATH "$ENV{WECORE_SRC}/Tests")
set(DSPFILTERS_CPP ${DSPFILTERS_PATH}/source/Biquad.cpp
                   ${DSPFILTERS_PATH}/source/Butterworth.cpp
                   ${DSPFILTERS_PATH}/source/Cascade.cpp
                   ${DSPFILTERS_PATH}/source/Filter.cpp
                   ${DSPFILTERS_PATH}/source/Param.cpp
                   ${DSPFILTERS_PATH}/source/PoleFilter.cpp)


add_executable(WECoreTest ${TEST_PATH}/catchMain.cpp
                          ${TEST_PATH}/AREnvelopeFollowerTests.cpp
                          ${TEST_PATH}/CarveDSPUnitTests.cpp
                          ${TEST_PATH}/CarveNoiseFilterTests.cpp
                          ${TEST_PATH}/MONSTRCrossoverTests.cpp
                          ${TEST_PATH}/RichterLFOPairTests.cpp
                          ${TEST_PATH}/SongbirdFilterModuleTests.cpp
                          ${TEST_PATH}/TPTSVFilterTests.cpp
                          ${DSPFILTERS_CPP})

add_executable(WECoreTestPerf ${TEST_PATH}/catchMain.cpp
                              ${TEST_PATH}/PerformanceTests.cpp
                              ${DSPFILTERS_CPP})