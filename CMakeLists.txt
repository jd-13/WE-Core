cmake_minimum_required(VERSION 3.0)
project(WECore)

include_directories($ENV{CATCH_PATH}
                    ${CMAKE_CURRENT_LIST_DIR}/WECore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GCOV_FLAGS -fprofile-arcs -ftest-coverage)

# Set the GCC and Clang flags
# TODO: work on removing the need for the below Wno flags
set(CXXFLAGS_CLANG -std=c++17 -g -Wall -Werror -Wextra -Wconversion -Wshadow -Weverything -Wpedantic -Wno-exit-time-destructors -Wno-weak-vtables -Wno-reserved-id-macro -Wno-double-promotion -Wno-unknown-warning-option -Wno-c++98-compat -Wno-padded -Wno-global-constructors)
set(CXXFLAGS_GCC   -std=c++17 -g -Wall -Werror -Wextra -Wconversion -Wshadow -pedantic ${GCOV_FLAGS})

# Set the flags we'll actually use based on the compiler
set(CXXFLAGS ${CXXFLAGS_CLANG})

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CXXFLAGS ${CXXFLAGS_GCC})
endif()

add_executable(WECoreTest ${CMAKE_CURRENT_LIST_DIR}/WECore/Tests/catchMain.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/CarveDSP/Tests/DSPUnitParameterTests.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/CarveDSP/Tests/DSPUnitProcessingTests.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/CarveDSP/Tests/CarveNoiseFilterTests.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/RichterLFO/Tests/RichterLFOPairTests.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/SongbirdFilters/Tests/SongbirdFilterModuleTests.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/WEFilters/Tests/AREnvelopeFollowerTests.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/WEFilters/Tests/SimpleCompressorTests.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/WEFilters/Tests/StereoWidthProcessorTests.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/WECore/WEFilters/Tests/TPTSVFilterTests.cpp)


add_executable(WECoreTestPerf ${CMAKE_CURRENT_LIST_DIR}/WECore/Tests/catchMain.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/WECore/Tests/PerformanceTests.cpp)

target_compile_options(WECoreTest     PRIVATE ${CXXFLAGS})
target_compile_options(WECoreTestPerf PRIVATE ${CXXFLAGS})

if (CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(WECoreTest     gcov)
    target_link_libraries(WECoreTestPerf gcov)
endif()