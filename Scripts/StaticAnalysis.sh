#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null 2>&1 && pwd )"
export WECORE_HOME="$(cd $SCRIPT_DIR/.. > /dev/null 2>&1 && pwd)"
export WECORE_SRC="$WECORE_HOME/WECore"

cd $WECORE_HOME

echo "=== Running clang-tidy ==="
clang-tidy -header-filter=.* \
    -checks=clang-analyzer-*,-clang-diagnostic-c++17-extensions,performance-* \
    $(find $WECORE_SRC -name *.cpp) -- \
    -I$WECORE_SRC \
    -I$CATCH_PATH \
    -I$WECORE_HOME/DSPFilters/shared/DSPFilters/include > clang-tidy.txt

NUM_CLANG_TIDY_WARNINGS=$(grep "warning:" clang-tidy.txt | wc -l)

echo "=== clang-tidy warnings: $NUM_CLANG_TIDY_WARNINGS ==="

if [ $NUM_CLANG_TIDY_WARNINGS != "0" ]; then
    exit 1
fi
