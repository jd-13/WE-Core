#!/bin/bash

set -e

BUILD_NUMBER=$1

GENERATE_COVERAGE=false
if [ "$CXX" = "/usr/bin/g++-10" ]; then
    GENERATE_COVERAGE=true
fi

echo "GENERATE_COVERAGE: $GENERATE_COVERAGE"

if [ $GENERATE_COVERAGE = true ]; then
    update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-10 90
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null 2>&1 && pwd )"
export WECORE_HOME="$(cd $SCRIPT_DIR/.. > /dev/null 2>&1 && pwd)"
export WECORE_SRC="$WECORE_HOME/WECore"

cd $WECORE_HOME
mkdir build && cd build
cmake .. && make

if [ $GENERATE_COVERAGE = true ]; then
    $WECORE_HOME/Scripts/get_code_cov.sh;
    bash <(curl -s https://codecov.io/bash);
fi

$VALGRIND_PATH/coregrind/valgrind --tool=callgrind ./WECoreTest

mv callgrind.out.* callgrind.out.$BUILD_NUMBER