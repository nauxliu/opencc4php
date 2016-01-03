#!/bin/sh
git clone https://github.com/BYVoid/OpenCC.git --depth 1
cd OpenCC

sudo make && sudo make install
phpize && ./configure && make clean && make