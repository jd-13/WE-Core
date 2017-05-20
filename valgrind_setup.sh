#!/bin/bash

svn co svn://svn.valgrind.org/valgrind/trunk valgrind

cd valgrind
./autogen.sh
./configure
make
sudo make install
