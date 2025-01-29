#!/bin/bash

set -eu

SCRIPT_DIR=`dirname $0`

function testKmeans() {
	RED="\033[31m"
	GREEN="\033[32m"
	RESET="\033[0m"

	k_and_maxIter=$1
	epsilon=$2
	inputFileName1=$3
	inputFileName2=$4
	expectedName=$5

	RUN_COMMAND="python3 kmeans_pp.py $k_and_maxIter ${epsilon} ${inputFileName1} ${inputFileName2}"
	echo "Running test >>>" ${RUN_COMMAND}
	eval ${RUN_COMMAND} | diff ${expectedName} - && echo -e "${GREEN}Test Passed.${RESET}" && return 0 || echo -e "${RED}Python TEST FAILED!!!${RESET}"
	echo "--- actual ---"
	eval ${RUN_COMMAND}
	echo "--- expected ---"
	cat ${expectedName}
	echo "________________"
}

pushd ${SCRIPT_DIR}/*_*_assignment2/
echo "------- Setup -------"
rm -r ./build || echo "No build to remove"
rm mykmeanssp.cpython*.so || echo "No so to remove"
echo "------- Compile -------"
python3 setup.py build_ext --inplace
echo "------- Test -------"
pytest
echo "------- Run -------"
testKmeans "3 333" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_1.txt
testKmeans "49 2" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_1__49_2_0.txt
testKmeans "2 2" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_1__2_2_0.txt
testKmeans "7" 0 ../tests/input_2_db_1.txt ../tests/input_2_db_2.txt ../tests/output_2.txt
testKmeans "15 750" 0 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt ../tests/output_3_fixed_rounding.txt
testKmeans "3 2" 0 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt ../tests/output_3__3_2_0.txt
testKmeans "3 3" 0 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt ../tests/output_3__3_3_0.txt
testKmeans "3 3" 1.91 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt ../tests/output_3__3_3_0.txt
testKmeans "3 3" 1.92 ../tests/input_3_db_1.txt ../tests/input_3_db_2.txt ../tests/output_3__3_3_1.92.txt

testKmeans "50 2" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_clusters.txt
testKmeans "1 2" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_clusters.txt
testKmeans "-2 2" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_clusters.txt
testKmeans "a 2" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_clusters.txt

testKmeans "2 1000" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_maxIter.txt
testKmeans "2 1" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_maxIter.txt
testKmeans "2 0" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_maxIter.txt
testKmeans "2 -2" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_maxIter.txt
testKmeans "2 a" 0 ../tests/input_1_db_1.txt ../tests/input_1_db_2.txt ../tests/output_invalid_maxIter.txt

testKmeans "2 2" 0 ../tests/input_invalid_db_1.txt ../tests/input_invalid_db_2.txt ../tests/output_general_error.txt
echo "------- End -------"
popd
