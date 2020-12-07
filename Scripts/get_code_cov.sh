#!/bin/bash
cd ${WECORE_SRC}
for filename in `find . | egrep '\.cpp'`;
do
    gcov-10 -n -o . $filename > /dev/null;
done
