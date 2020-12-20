#!/bin/bash

if [[ $1 == "clean" ]]
then
    rm -rf data_dir
fi
mkdir -p data_dir
./build/apps/btdb
