#!/bin/bash

cd ..
make
cd -

./nachos -x ../test/shell
