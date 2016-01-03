apt-get install doxygen

#!/bin/sh
git clone https://github.com/BYVoid/OpenCC.git --depth 1
cd OpenCC

make && make test && make install

cd .. && phpize && ./configure && make clean && make