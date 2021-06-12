#!/bin/bash
cd ${WECORE_SRC}
for filename in `find . | egrep '\.cpp'`;
do
    gcov-10 -o ../build/CMakeFiles/WECoreTest.dir/WECore/$filename.o $filename
done
