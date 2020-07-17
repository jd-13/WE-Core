#!/bin/bash

update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-7 90

export WECORE_HOME=`pwd`
export WECORE_SRC="$WECORE_HOME/WECore"

cd $WECORE_HOME
mkdir build && cd build
cmake .. && make
if [ "$CXX" = "g++-7" ]; then
    $WECORE_HOME/Scripts/get_code_cov.sh;
fi

$VALGRIND_PATH/coregrind/valgrind --tool=callgrind ./WECoreTest
if [ "$CXX" = "g++-7" ]; then
    bash <(curl -s https://codecov.io/bash);
fi

mv callgrind.out.* callgrind.out.$BUILD_NUMBER