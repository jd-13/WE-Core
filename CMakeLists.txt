cmake_minimum_required(VERSION 3.0)
project(WECore)

set(DSPFILTERS_PATH $ENV{WECORE_HOME}/DSPFilters/shared/DSPFilters)

include_directories($ENV{CATCH_PATH}
                    $ENV{WECORE_SRC}
                    ${DSPFILTERS_PATH}/include)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GCOV_FLAGS -fprofile-arcs -ftest-coverage)

# Set the GCC and Clang flags
# TODO: work on removing the need for the below Wno flags
set(CXXFLAGS_CLANG -std=c++17 -g -Wall -Werror -Wextra -Wconversion -Wshadow -Weverything -Wpedantic -Wno-exit-time-destructors -Wno-weak-vtables -Wno-reserved-id-macro -Wno-double-promotion -Wno-unknown-warning-option -Wno-c++98-compat -Wno-padded -Wno-global-constructors)
set(CXXFLAGS_GCC   -std=c++17 -g -Wall -Werror -Wextra -Wconversion -Wshadow -pedantic ${GCOV_FLAGS})

# Set the GCC and Clang flags for the DSPFilters library
set(CXXFLAGS_CLANG_DSP ${CXXFLAGS_CLANG} -Wno-float-equal -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-old-style-cast -Wno-padded -Wno-unused-variable -Wno-sign-conversion -Wno-missing-noreturn -Wno-zero-as-null-pointer-constant -Wno-extra-semi-stmt)
set(CXXFLAGS_GCC_DSP ${CXXFLAGS_GCC} -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-variable -Wno-sign-conversion -Wno-missing-noreturn)

# Set the flags we'll actually use based on the compiler
set(CXXFLAGS ${CXXFLAGS_CLANG})
set(CXXFLAGS_DSP ${CXXFLAGS_CLANG_DSP})

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CXXFLAGS ${CXXFLAGS_GCC})
    set(CXXFLAGS_DSP ${CXXFLAGS_GCC_DSP})
endif()

add_library(dspfilters STATIC
            ${DSPFILTERS_PATH}/source/Biquad.cpp
            ${DSPFILTERS_PATH}/source/Butterworth.cpp
            ${DSPFILTERS_PATH}/source/Cascade.cpp
            ${DSPFILTERS_PATH}/source/Filter.cpp
            ${DSPFILTERS_PATH}/source/Param.cpp
            ${DSPFILTERS_PATH}/source/PoleFilter.cpp)

add_executable(WECoreTest $ENV{WECORE_SRC}/Tests/catchMain.cpp
                          $ENV{WECORE_SRC}/CarveDSP/Tests/DSPUnitParameterTests.cpp
                          $ENV{WECORE_SRC}/CarveDSP/Tests/DSPUnitProcessingTests.cpp
                          $ENV{WECORE_SRC}/CarveDSP/Tests/CarveNoiseFilterTests.cpp
                          $ENV{WECORE_SRC}/MONSTRFilters/Tests/MONSTRCrossoverTests.cpp
                          $ENV{WECORE_SRC}/RichterLFO/Tests/RichterLFOPairTests.cpp
                          $ENV{WECORE_SRC}/SongbirdFilters/Tests/SongbirdFilterModuleTests.cpp
                          $ENV{WECORE_SRC}/WEFilters/Tests/AREnvelopeFollowerTests.cpp
                          $ENV{WECORE_SRC}/WEFilters/Tests/TPTSVFilterTests.cpp)

add_executable(WECoreTestPerf $ENV{WECORE_SRC}/Tests/catchMain.cpp
                              $ENV{WECORE_SRC}/Tests/PerformanceTests.cpp)

target_compile_options(dspfilters     PRIVATE ${CXXFLAGS_DSP})
target_compile_options(WECoreTest     PRIVATE ${CXXFLAGS})
target_compile_options(WECoreTestPerf PRIVATE ${CXXFLAGS})

if (CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(WECoreTest     dspfilters gcov)
    target_link_libraries(WECoreTestPerf dspfilters gcov)
else()
    target_link_libraries(WECoreTest     dspfilters)
    target_link_libraries(WECoreTestPerf dspfilters)
endif()