#!/bin/sh

git clone https://github.com/BYVoid/OpenCC.git --depth 1

cd OpenCC
make && make test && make install
cd ..