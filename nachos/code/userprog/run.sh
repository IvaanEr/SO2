#!/bin/bash

cd ..
make
cd -

clear

./nachos -d p -x ../test/shell
