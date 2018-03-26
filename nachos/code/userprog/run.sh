#!/bin/bash

cd ..
make
cd -

clear

./nachos  -x ../test/shell
