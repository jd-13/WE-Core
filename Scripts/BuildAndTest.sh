#!/bin/bash

set -e

GENERATE_COVERAGE=false

if [ "$CXX" = "/usr/bin/g++-10" ]; then
    GENERATE_COVERAGE=true
fi

echo "=== Compiler is $CXX ==="
echo "=== Setting GENERATE_COVERAGE: $GENERATE_COVERAGE ==="

if [ $GENERATE_COVERAGE = true ]; then
    update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-10 90
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null 2>&1 && pwd )"
export WECORE_HOME="$(cd $SCRIPT_DIR/.. > /dev/null 2>&1 && pwd)"
export WECORE_SRC="$WECORE_HOME/WECore"

echo "=== Starting build ==="

cd $WECORE_HOME
mkdir -p build && cd build
cmake .. && make

echo "=== Starting tests ==="
valgrind --tool=callgrind ./WECoreTest

if [ $GENERATE_COVERAGE = true ]; then
    echo "=== Generating coverage ==="
    $WECORE_HOME/Scripts/get_code_cov.sh;

    echo "=== Uploading coverage report ==="
    curl -Os https://uploader.codecov.io/latest/codecov-linux
    chmod +x codecov-linux
    ./codecov-linux
fi

echo "=== Renaming callgrind output ==="
mv callgrind.out.* callgrind.out.$(git log --pretty=format:'%h' -n 1)
