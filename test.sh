#/bin/bash

FLAGS="counter<recursion>
counter<folding>
is_unique<recursion>
is_unique<folding>
index_of<recursion>
index_of<folding>
index_of<folding2>"

TEST_PATH=test_$CXX

if  [ -e ${TEST_PATH} ]; then
  rm -rf ${TEST_PATH}
fi

for((j=0; j<10; j=j+1)); do
  mkdir -p ${TEST_PATH}/$j
  for FLAG in $FLAGS; do
    echo "${j} ${FLAG}"
    g++ -o gen -DCURRENT_TAG=$FLAG gen.cpp
    for ((i=0; i<=100; i=i + 10)); do
      ./gen $i
      /usr/bin/time -f "$i\t%e\t%M" $CXX -std=c++1z -O0 -c gen_main.cpp 2>> ${TEST_PATH}/$j/$FLAG
    done
  done
done
