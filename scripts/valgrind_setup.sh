#!/bin/bash

svn co svn://svn.valgrind.org/valgrind/trunk /home/valgrind

cd /home/valgrind
./autogen.sh
./configure
make
make install
