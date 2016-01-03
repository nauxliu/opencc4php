#!/bin/sh
apt-get install doxygen

git clone https://github.com/BYVoid/OpenCC.git --depth 1

cd OpenCC
make && make test && make install

cd ..
phpize && ./configure && make clean && make
