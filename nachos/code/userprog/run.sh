#!/bin/bash

cd ..
make
cd -

./nachos -d p -x ../test/shell
