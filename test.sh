#!/bin/bash

# shellcheck disable=SC2164
# shellcheck disable=SC2103

function clean() {
    rm test/test_d.bin
    rm test/test_d.log
    rm test/test_d.yo
    make clean
}

cd bin
echo -e "\033[1;34m\nGenerating assembly file......\033[0m"
./yas ../test/test_d.ys

cd ..
echo -e "\033[1;34m\nGenerating binary file......\033[0m"
python3 assembler.py -i test/test_d.yo -o test/test_d.bin

echo -e "\033[1;34m\nCompiling yie file......\033[0m"
make

cd bin
echo -e "\033[1;34m\nRunning tests......\033[0m"
./yie ../test/test_d.bin > ../test/test_d.log

if (grep "Suspected dead cycle, please check your program!" ../test/test_d.log > /dev/null)
then
    cd ..
    clean > /dev/null
    echo -e "\033[1;32;43m Dead cycle Test passed \033[0m"
else
    cd..
    clean > /dev/null
    echo -e "\033[1;31;43m Dead cycle Test failed \033[0m"
fi
