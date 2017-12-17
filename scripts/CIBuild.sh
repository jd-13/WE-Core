#!/bin/bash

if [ "$CXX" = "g++" ]; then export CXX="g++-5" CC="gcc-5"; fi
if [ "$CXX" = "clang++" ]; then export CXX="clang++-5.0" CC="clang-5.0"; fi
update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-5 90

git submodule init
git submodule update
export WECORE_SRC=/home/WE-Core

mkdir build && cd build
cmake .. && make
if [ "$CXX" = "g++-5" ]; then $WECORE_SRC/scripts/get_code_cov.sh; fi

$VALGRIND_PATH/coregrind/valgrind --tool=callgrind ./WECoreTest
if [ "$CXX" = "g++-5" ]; then bash <(curl -s https://codecov.io/bash); fi
cppcheck -i$WECORE_SRC/DSPFilters -i$WECORE_SRC/valgrind --quiet --error-exitcode=1 .
mv callgrind.out.* /home/travisDir/callgrind.out.$TRAVIS_BUILD_NUMBER