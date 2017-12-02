#!/bin/bash

if [ "$CXX" = "g++" ]; then export CXX="g++-5" CC="gcc-5"; fi
if [ "$CXX" = "clang++" ]; then export CXX="clang++-5.0" CC="clang-5.0"; fi
update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-5 90

scripts/valgrind_setup.sh
git submodule init
git submodule update

make
if [ "$CXX" = "g++-5" ]; then scripts/get_code_cov.sh; fi

valgrind/coregrind/valgrind --tool=callgrind ./WECoreTest
if [ "$CXX" = "g++-5" ]; then bash <(curl -s https://codecov.io/bash); fi
cppcheck -iDSPFilters -ivalgrind --quiet --error-exitcode=1 .
#mv callgrind.out.* callgrind.out.$TRAVIS_BUILD_NUMBER